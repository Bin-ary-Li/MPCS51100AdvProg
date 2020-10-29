#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 4
#define SOFTENING 1e-9f

typedef struct {
    int id;
    float x,y;
    float vx,vy;
    float Fx,Fy;
} Particle;

typedef struct {
    float Fx, Fy;
} Force; 

typedef struct qtnode_ {
    Particle *particle;
    int hasChild;
    int which_child;
    float size;
    float center_of_mass_x_sum;
    float center_of_mass_y_sum;
    float total_mass;
    float lb,rb,ub,db;
    struct qtnode_* parent;
    struct qtnode_* child[N];
} QTNode;

QTNode * create_root(float size);
QTNode * create_node(QTNode *parent, int child_index);
void resize_root(QTNode *root, float size);
int is_particle_equal(Particle *p1, Particle *p2);
static int is_in_box(Particle *p, QTNode *n);
static void update_mass(Particle *p, QTNode *n);
QTNode* which_child_contains(QTNode *n, Particle *p);
void qTree_insert(Particle *p, QTNode *n);
char * toStr(const void * n, char * s);
void qTree_print_dfs(QTNode * root);
void qTree_print_bfs(QTNode * root);
Force * qTree_calcForce(Particle *p, QTNode *n, float theta);
void print_particle(Particle *p, int len);
void lattice_init(Particle *p, int n);
void ran_init(Particle *p, int numP, float size);
void twobox_init(Particle *p, int numP, float size);
void twocircle_init(Particle *p, int numP, float size);
int compare_particle(Particle *p, Particle *refP, int len, float epsi);
void calc_force(Particle *p, float dt, int n);
QTNode * qTree_clear(QTNode *node);
