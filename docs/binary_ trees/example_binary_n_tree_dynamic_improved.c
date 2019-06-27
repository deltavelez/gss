#include<stdlib.h>
#include<stdio.h>

struct tree_node 
{
  int depth;
  int min_x;
  int max_x;
  int n_children;
  struct tree_node **args;
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

void init_root(tree_node_t *parent, int n_args)
{
  int i;
  parent = (tree_node_t *)malloc(sizeof(tree_node_t));
  parent->min_x=1;
  parent->max_x=80;
  parent->depth=1;
  parent->n_children=n_args;
  parent->args=(tree_node_t **)malloc(n_args*sizeof(tree_node_t));
  for(i=0;i<n_args;i++)
    parent->args[i]=NULL;
}

void create_subnodes(tree_node_t *parent, int n_args) 
{
  int i;
  tree_node_t *temp_child;

  parent->n_children=n_args;
  parent->args =(tree_node_t *)malloc(n_args*sizeof(tree_node_t));

  for(i=0;i<n_args;i++)
    {
      temp_child= (tree_node_t *)malloc(sizeof(tree_node_t));
      temp_child->depth=parent->depth+1;
      temp_child->min_x = parent-> min_x +     i*(parent->max_x-parent->min_x)/parent->n_children;
      temp_child->max_x = parent-> min_x + (i+1)*(parent->max_x-parent->min_x)/parent->n_children;
      temp_child->n_children=0;
      temp_child->args=NULL;
      parent->args[i]=temp_child;
  
      //printf("depth = %d\r\n",node->depth);
    }
  return;
}

void print_node(tree_node_t *tree) 
{
  printf("node address = %i\r\n",tree);
  if(tree==NULL)
    printf("node is NULL\r\n");
  else
    {
      printf("node->min_x = %i\r\n",tree->min_x);
      printf("node->max_x = %i\r\n",tree->max_x);
      printf("node->depth = %i\r\n",tree->depth);
      printf("node->n_children = %i\r\n",tree->n_children);
    }
}


void printout(tree_node_t *tree) 
{
  int i;
  gotoxy(0.5*(tree->min_x+tree->max_x),tree->depth);
  printf("%i,%i\n", tree->depth, tree->n_children);
   //printf("[%i,%i],%i\n", tree->min_x, tree->max_x, tree->depth);
  
  for (i=0;i<tree->n_children;i++)
    printout(tree->args[i]);
}

void main() 
{
  tree_node_t  *root = NULL;
  int i;

  cls();
  /* Initializing the main node */
  root= (tree_node_t *)malloc(sizeof(tree_node_t));
  root->min_x=1;
  root->n_children=0;
  root->max_x=80;
  root->depth=1;
  /* End of initializing the main node */
  
  create_subnodes(root,10);
  create_subnodes(root->args[5],1);
  //create_subnodes(root->args[1],3);
  //create_subnodes(root->args[1]->args[1],2);
  //create_subnodes(root->args[1]->args[1]->args[1],1);
 
 
   
  printout(root);
  printf("\n");
  printf("\n");
  printf("\n");
  printf("\n");
  printf("\n");
}

