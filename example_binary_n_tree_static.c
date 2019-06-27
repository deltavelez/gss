#include<stdlib.h>
#include<stdio.h>

struct tree_node 
{
  int min_x;
  int max_x;
  int depth;
  int n_children;
  struct tree_node *args[10];
};

typedef struct tree_node tree_node_t;

int powint(int k, int n)
{
  int temp, i;
  if (n==0) 
    return 1;
  temp=k;
  for (i=1;i<n;i++)
    temp*=k;
  return temp;
}

void gotoxy(int x,int y)
{
  printf("%c[%d;%df",0x1B,y,x);
}

void cls(void)
 {
   printf("%c[%dJ",0x1B,2);
 }

void insert_node(tree_node_t *parent, int branch, int n_args) 
{
  int i;
  
  tree_node_t *temp_child;
  temp_child= (tree_node_t *)malloc(sizeof(tree_node_t));
  temp_child->min_x = parent->min_x + branch    *(parent->max_x-parent->min_x)/parent->n_children;
  temp_child->max_x = parent->min_x + (branch+1)*(parent->max_x-parent->min_x)/parent->n_children;
  temp_child->n_children=n_args;
  temp_child->depth=parent->depth+1;
  for(i=0;i<n_args;i++)
    temp_child->args[i]=NULL;

  parent->args[branch]=temp_child;
  //printf("depth = %d\r\n",node->depth);
  return;
}

void printout(tree_node_t *tree) 
{
  int i;
  gotoxy(0.5*(tree->min_x+tree->max_x),tree->depth);
  printf("[%i,%i]\n", tree->min_x, tree->max_x);
  
  for (i=0;i<tree->n_children;i++)
    if(tree->args[i] != NULL)
      printout(tree->args[i]);
}

void main() 
{
  tree_node_t  *root = NULL;
  int i;
  cls();
  root= (tree_node_t *)malloc(sizeof(tree_node_t));
  root->min_x=1;
  root->max_x=80;
  root->depth=1;
  root->n_children=3;
  for(i=0;i<3;i++)
    root->args[i]=NULL;
 
  insert_node(root, 0,2);
  insert_node(root, 1,1);
  insert_node(root, 2,1);
  insert_node(root->args[0], 0,1);
  insert_node(root->args[0], 1,1);
   
  printout(root);
  printf("\n");
  printf("\n");
  printf("\n");
  printf("\n");
  printf("\n");
}

/*

   for(i=1;i<=10;i++) {
      curr = (node *)malloc(sizeof(node));
      curr->left = curr->right = NULL;
      curr->val = i;
      insert(&root, curr);
   }

*/
