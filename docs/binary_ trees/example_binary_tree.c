#include<stdlib.h>
#include<stdio.h>

typedef enum {left, right} branch_t;

struct tree_node 
{
  int pos_x;
  int depth;
  struct tree_node *r;
  struct tree_node *l;
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

void insert(tree_node_t *node, branch_t branch) 
{
  //printf("depth = %d\r\n",node->depth);
  
  tree_node_t *temp;
  int test;
  temp= (tree_node_t *)malloc(sizeof(tree_node_t));
  temp->l=NULL;
  temp->r=NULL;
  temp->depth=node->depth+1;
  test=node->depth;
  if (branch==left)
    {
      temp->pos_x=node->pos_x-40/powint(2,temp->depth-1);
      
      node->l=temp;
    }
  else
    {
      temp->pos_x=node->pos_x+40/powint(2,temp->depth-1);
      node->r=temp;
    }
  return;
}

void printout(tree_node_t *tree) 
{
   gotoxy(tree->pos_x,tree->depth);
   printf("%d,%d\n",tree->pos_x,tree->depth);
   if((tree->l) != NULL)  printout(tree->l);
   if((tree->r) != NULL)  printout(tree->r);
}

void main() 
{
  tree_node_t  *root = NULL;
  cls();
  root= (tree_node_t *)malloc(sizeof(tree_node_t));
  root->pos_x=40;
  root->depth=1;
  root->l=NULL;
  root->r=NULL;
  
  insert(root, left);
  insert(root, right);
  insert(root->l, left);
  insert(root->l->l, right);
   
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
