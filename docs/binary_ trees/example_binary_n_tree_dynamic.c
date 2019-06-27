#include<stdlib.h>
#include<stdio.h>

struct tree_node 
{
  int min_x;
  int max_x;
  int depth;
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

void insert_node(tree_node_t *parent, int branch, int n_args) 
{
  int i;
  tree_node_t *temp_child;

  if(parent->n_children==0)
    {
      parent->n_children=n_args;
      parent->args=(tree_node_t **)malloc(n_args*sizeof(tree_node_t));
      for(i=0;i<n_args;i++)
	parent->args[i]=NULL;
    }
  else
    {
      if (branch>=parent->n_children)
	{
	  printf("error: invalid branch\r\n");
	  return;
	}
      temp_child= (tree_node_t *)malloc(sizeof(tree_node_t));
      temp_child->min_x = parent->min_x + branch    *(parent->max_x-parent->min_x)/parent->n_children;
      temp_child->max_x = parent->min_x + (branch+1)*(parent->max_x-parent->min_x)/parent->n_children;
      temp_child->n_children=n_args;
      temp_child->args=(tree_node_t **)malloc(n_args*sizeof(tree_node_t));
      temp_child->depth=parent->depth+1;
      for(i=0;i<n_args;i++)
	temp_child->args[i]=NULL;
  
      parent->args[branch]=temp_child;
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
  init_root(root, 3); 
  root= (tree_node_t *)malloc(sizeof(tree_node_t));
  root->min_x=1;
  root->max_x=80;
  root->depth=1;
  //root->n_children=3;
  //root->args=(tree_node_t **)malloc(root->n_children*sizeof(tree_node_t));
  //for(i=0;i<root->n_children;i++)
  //  root->args[i]=NULL;
 
  insert_node(root, 0,2);
  insert_node(root, 0,1);
  insert_node(root, 1,1);
  insert_node(root->args[1], 0,1);
  insert_node(root->args[1]->args[0], 0,2);
  insert_node(root->args[1]->args[0]->args[0], 0,1);
  //insert_node(root->args[1]->args[0]->args[1], 1,1);
  //insert_node(root, 2,1);
  //insert_node(root->args[0], 0,1);
  //insert_node(root->args[0], 1,1);
   
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
