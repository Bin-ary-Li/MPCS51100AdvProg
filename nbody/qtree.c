#include "qtree.h"
#include "list.h"

QTNode * create_root(float size) {
    QTNode * node = malloc(sizeof(QTNode));
    node->parent = NULL;
    node->particle = NULL;
    node->hasChild = 0 ; node->total_mass = 0; node->center_of_mass_x_sum = 0; node->center_of_mass_y_sum = 0;
    node->size = size;
    node->lb = -(size/2.0); node->rb = (size/2.0); 
    node->db = -(size/2.0); node->ub = (size/2.0);
}


QTNode * create_node(QTNode *parent, int child_index) {
    QTNode * node = malloc(sizeof(QTNode));
    node->parent = parent;
    node->particle = NULL;
    node->hasChild = 0 ; node->total_mass = 0; node->center_of_mass_x_sum = 0; node->center_of_mass_y_sum = 0;
    for (int i =0; i<N; ++i) {
        node->child[i] = NULL;
    }

    if (parent == NULL) { // if creating a root node
        printf("Use create_root to create root node with no parent.\n");
        return NULL;
    } else {
        node->size = .5*node->parent->size;
        node->which_child = child_index;
        parent->child[child_index] = node;
        if (node->which_child == 0){
        node->lb   = node->parent->lb; node->rb   = node->lb + node->size;
        node->ub   = node->parent->ub; node->db   = node->ub - node->size;
        }
        else if (node->which_child == 1){
        node->rb   = node->parent->rb; node->lb   = node->rb - node->size;
        node->ub   = node->parent->ub; node->db   = node->ub - node->size;
        }
        else if (node->which_child == 2){
        node->lb   = node->parent->lb; node->rb   = node->lb + node->size;
        node->db   = node->parent->db; node->ub   = node->db + node->size;
        }
        else if (node->which_child == 3){
        node->rb   = node->parent->rb; node->lb   = node->rb - node->size;
        node->db   = node->parent->db; node->ub   = node->db + node->size;
        }
    }
    return node;
}

// Resize the domain of a clean root
void resize_root(QTNode *root, float size) {
    if (root == NULL) return;
    if (root->hasChild) {
        printf("Resize fail, root has children");
        return;
    }
    root->size = size;
    root->lb = -(size/2.0); root->rb = size/2.0; root->db = -(size/2.0); root->ub = size/2.0;
}

int is_particle_equal(Particle *p1, Particle *p2) {
    if (p1==NULL || p2==NULL) return 0;
    if (p1->x == p2->x && p1->y == p2->y) return 1;
    return 0;
}

static int is_in_box(Particle *p, QTNode *n) {
    if (p == NULL || n == NULL) return 0;
    
    if (p->x >= n->lb && p->x < n->rb && p->y >= n->db && p->y < n->ub) {
        return 1;
    } else {
        return 0;
    }
}

static void update_mass(Particle *p, QTNode *n) {
    // Assuming unit mass, the total mass is just the count of particles and the center of mass(x,y) can be given by:
    // x = (x_1+...+x_n)/total_mass, y = (y_1+...+y_n)/total_mass
    // We keep track of these two sums in center_of_mass_x and center_of_mass_y. 
    ++(n->total_mass); 
    n->center_of_mass_x_sum += p->x;
    n->center_of_mass_y_sum += p->y;
}

QTNode* which_child_contains(QTNode *n, Particle *p) {
    if (n == NULL || p == NULL) return NULL;
    if (n->hasChild != 1) return NULL; 
    QTNode *child;
    for(int i=0; i<N; ++i) {
        child = n->child[i];
        if (is_in_box(p, child)) {
            return child;
        }
    }
}

void qTree_insert(Particle *p, QTNode *n) {
    if (p == NULL || n == NULL) return;

    if (n->hasChild) { // if n has children, insert particle to its children
        QTNode * child = which_child_contains(n, p);
        qTree_insert(p, child);

    } else if (n->particle != NULL) { // if no children and also not a empty node, create children
        if (is_particle_equal(n->particle, p)){
            // printf("Warning: inserting two identical particles into tree, merging two particles.\n");
            update_mass(p,n);
            return;
            // exit(1);
        }
        for(int i=0; i<N; i++){ // add 4 child and move the previously stored particle
            QTNode *node = create_node(n, i);
            if (is_in_box(n->particle, node)) {
                node->particle = n->particle;
                update_mass(node->particle, node);
                n->particle = NULL;
            }
        }
        n->hasChild = 1; // mark n as having children
        QTNode * child = which_child_contains(n, p); // find the child that should hold the particle that need to be stored
        qTree_insert(p, child);
    } else {
        n->particle = p;
    }
    update_mass(p, n);
}

char * toStr(const void * n, char * s) {
    QTNode * node = (QTNode *) n;
    if (node->particle != NULL) {
        sprintf(s, "%f, %f", node->particle->x,node->particle->y);
    } else {
        sprintf(s, " ");
    }
    return s;
}

// Print out the tree by DFS traversal, uses linked list from hw1 to implement stack for easy DFS
void qTree_print_dfs(QTNode * root) {
    List *l = list_create(toStr, NULL, NULL, 10);
    list_append(l, root);
    while (l->size > 0) {
        QTNode * node = list_remove(l, 0);
        
        if (node == NULL) {
            printf("[EmptyNode]");
            continue;
        }

        if (node->particle != NULL) {
            printf("(%f,%f) in ", node->particle->x, node->particle->y);
            printf("node range [lb:%f,rb:%f,ub:%f,db:%f] w/ TM: %f\n", node->lb, node->rb, node->ub, node->db, node->total_mass);
        } else if (node->hasChild) {
            printf("(TM: %f, CoM: (%f,%f))\n", node->total_mass, node->center_of_mass_x_sum/node->total_mass, node->center_of_mass_y_sum/node->total_mass);
        } else {
            printf("(EmptyChild)\n");
        }

        if (node->hasChild) {
            for(int i=0; i<N; ++i) {
                list_prepend(l, node->child[i]);
            }
        } 
    }
    list_destroy(&l);
}

// Print out the tree by BFS traversal, uses linked list from hw1 to implement queue for easy BFS
void qTree_print_bfs(QTNode * root) {
    List *l = list_create(toStr, NULL, NULL, 10);
    list_append(l, root);
    while (l->size > 0) {
        QTNode * node = list_remove(l, 0);
        
        if (node == NULL) {
            printf("[EmptyNode]");
            continue;
        }
        if (node->particle != NULL) {
            printf("(%f,%f) in ", node->particle->x, node->particle->y);
            printf("node range [lb:%f,rb:%f,ub:%f,db:%f] w/ TM: %f\n", node->lb, node->rb, node->ub, node->db, node->total_mass);
        } else if (node->hasChild) {
            printf("(TM: %f, CoM: (%f,%f), range: [lb:%f,rb:%f,ub:%f,db:%f])\n", 
            node->total_mass, node->center_of_mass_x_sum/node->total_mass, node->center_of_mass_y_sum/node->total_mass, 
            node->lb, node->rb, node->ub, node->db);
        } else {
            printf("(EmptyChild)\n");
        }

        if (node->hasChild) {
            for(int i=0; i<N; ++i) {
                list_append(l, node->child[i]);
            }
        } 
    }
    list_destroy(&l);
}

// Compute the net force on a particle by all bodies in a node
Force * qTree_calcForce(Particle *p, QTNode *n, float theta) {
    if (p == NULL || n == NULL) {
        printf("Error: compute on null particle or node, exiting\n");
        exit(1);
    }
    Force * f = malloc(sizeof(Force));
    f->Fx = 0.0; f->Fy = 0.0; 
    if (n->particle != NULL) { // evaluate force directly if there is only one particle in node.

        float dx = n->particle->x - p->x;
        float dy = n->particle->y - p->y;
        float distSqr = ((dx*dx) + dy*dy) + SOFTENING;
        float invDist = 1.0f / sqrtf(distSqr);
        float invDist3 = (invDist * invDist) * invDist;
        f->Fx += dx * invDist3 * n->total_mass; f->Fy += dy * invDist3 * n->total_mass; // When there be issue of overlapping, i.e. n->total_mass > 1
    } else if (n->hasChild) { // if the node has children, try to evaluate using CM and TM
        float cm_x = n->center_of_mass_x_sum/n->total_mass;
        float cm_y = n->center_of_mass_y_sum/n->total_mass;
        float dx = cm_x - p->x;
        float dy = cm_y - p->y;
        float r = sqrtf(dx*dx + dy*dy); // calculate distance from particle to node's center of mass;
        float d = n->size;
        if (d/r <= theta) { // ok to approximate all bodies in this node using CM and TM
            float distSqr = dx*dx + dy*dy + SOFTENING;
            float invDist = 1.0 / sqrtf(distSqr);
            float invDist3 = invDist * invDist * invDist;
            f->Fx += dx * invDist3; f->Fy += dy * invDist3;
        } else {
            for (int i=0;i<N;++i) { // recursively checking for all children of the node
                Force * returnF = qTree_calcForce(p, n->child[i], theta);
                f->Fx += returnF->Fx; f->Fy += returnF->Fy;
                free(returnF);  
            }
        }
    } 
    return f;
}

void print_particle(Particle *p, int len) {
    printf("[");
    for (int i=0; i<len;++i) {
        printf("i=%d(%f,%f) ", i, p[i].x, p[i].y);
    }
    printf("]\n");
}

// Quickly initialize a list of particles in lattice layout, n should be a square number.
void lattice_init(Particle *p, int n) {
    int i,j;
    float dx, dy;
    int m = (int) sqrt(n); 
    int cnt = 0;
    dx = dy = 1./m;
    for (i=0;i<m;++i){
        for(j=0;j<m;++j){
            p[j+m*i].id = cnt;
            p[j+m*i].x = dx*(j+.5);
            p[j+m*i].y = dy*(i+.5);
            cnt++;
        }
    }
}

void ran_init(Particle *p, int numP, float size) {
  for (int i = 0; i < numP; i++) {
      p[i].id = i;
      p[i].x = size * (rand() / (float)RAND_MAX) - size/2.0;
      p[i].y = size * (rand() / (float)RAND_MAX) - size/2.0;
      p[i].vx = size * (rand() / (float)RAND_MAX) - size/2.0;
      p[i].vy = size * (rand() / (float)RAND_MAX) - size/2.0;
  }
}

float normal_ran(float m, float s)	/* normal random variate generator */
{				        /* mean m, standard deviation s */
    float v1 = (rand() + 1.0) / ((float)RAND_MAX + 1.0);
    float v2 = (rand() + 1.0) / ((float)RAND_MAX + 1.0);

	float ran = cos(2*3.14*v2)*sqrt(-2.*log(v1));

	return( m + ran * s );
}

//  Particles will be initialized and put into two boxes
void twobox_init(Particle *p, int numP, float size) {
    int mid = (int) numP/2;
    int cnt = 0;
    for (int i = 0; i < mid; i++) {
        p[i].id = cnt;
        p[i].x = size/4.0 * (rand() / (float)RAND_MAX);
        p[i].y = size/4.0 * (rand() / (float)RAND_MAX);
        p[i].vx = (rand() / (float)RAND_MAX);
        p[i].vy = (rand() / (float)RAND_MAX);
        ++cnt;
    }
    for (int i = mid; i < numP; i++) {
        p[i].id = cnt;
        p[i].x = -size/4.0 * (rand() / (float)RAND_MAX);
        p[i].y = -size/4.0 * (rand() / (float)RAND_MAX);
        p[i].vx = (rand() / (float)RAND_MAX);
        p[i].vy = (rand() / (float)RAND_MAX);
        ++cnt;
    }
}

//  Particles will be initialized and put into two circle
void twocircle_init(Particle *p, int numP, float size) {
    int mid = (int) numP/2;
    int cnt = 0;
    for (int i = 0; i < mid; i++) {
        p[i].id = cnt;
        p[i].x = normal_ran(size/4.0, 1.0);
        p[i].y = normal_ran(size/4.0, 1.0);
        p[i].vx = (rand() / (float)RAND_MAX);
        p[i].vy = (rand() / (float)RAND_MAX);
        ++cnt;
    }
    for (int i = mid; i < numP; i++) {
        p[i].id = cnt;
        p[i].x = normal_ran(-size/4.0, 1.0);
        p[i].y = normal_ran(-size/4.0, 1.0);
        p[i].vx = (rand() / (float)RAND_MAX);
        p[i].vy = (rand() / (float)RAND_MAX);
        ++cnt;
    }
}

int compare_particle(Particle *p, Particle *refP, int len, float epsi) {
  int errCnt = 0;
  for (int i=0; i<len; ++i) {
      if (fabsf(p[i].x-refP[i].x) > epsi || fabsf(p[i].y-refP[i].y) > epsi) {
          ++errCnt;
      }
  }
  return errCnt;
}

void calc_force(Particle *p, float dt, int n) {
  for (int i = 0; i < n; i++) { 
    float Fx = 0.0; float Fy = 0.0;
    for (int j = 0; j < n; j++) {
      /* calculate net particle for on i'th particle */
      float dx = p[j].x - p[i].x;
      float dy = p[j].y - p[i].y;
      float distSqr = ((dx*dx) + dy*dy) + SOFTENING;
      float invDist = 1.0f / sqrtf(distSqr);
      float invDist3 = (invDist * invDist) * invDist;

      Fx += dx * invDist3; Fy += dy * invDist3;
    }
    /* update instantaneous velocity based on force and timestep */
    // printf("i=%d(Fx=%f,Fy=%f)\n", i, Fx, Fy);
    p[i].Fx = Fx; p[i].Fy = Fy;
    p[i].vx = dt*Fx; p[i].vy = dt*Fy;
  }
}

// Clearing all children of a node
QTNode * qTree_clear(QTNode *node) {
    if (node == NULL) return NULL;
    if (node->hasChild == 0) {
        printf("Node does not have any children\n");
        return NULL;
    }

    List *l = list_create(toStr, NULL, NULL, 10);
    for(int i=0; i<N; ++i) {
        list_prepend(l, node->child[i]);
        node->child[i] = NULL;
    }
    node->hasChild = 0; node->total_mass = 0.0f; node->center_of_mass_y_sum = 0.0f; node->center_of_mass_x_sum = 0.0f;
    while (l->size > 0) {
        QTNode * n = list_remove(l, 0);
        if (n->hasChild) {
            for(int i=0; i<N; ++i) {
                list_prepend(l, n->child[i]);
            }
        }
        free(n);
    }
    list_destroy(&l);
    return node;
}
