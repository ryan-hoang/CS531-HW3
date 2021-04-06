/*
Ryan Hoang
G00973915
A simple program that reads a data file with addresses and associated aliases.
Allows the user to add, lookup, update, delete, display the location of,
and save this data to a file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct address_t
{
  int first;
  int second;
  int third;
  int fourth;
  char alias[11];
  struct address_t *left, *right;
};
struct address_t* root = NULL;

//==============================Prototypes======================================
void read_in_data(); //Initialize the list of address/alias pairs
void add_address(char* address, char* alias); //Insert an address/alias pair
int alias_exists(struct address_t* temp, char* alias);//Check if alias is already present

//Check if address is already present.
int address_exists(struct address_t* temp,int first,int second, int third, int fourth);

//Finds and returns a node in the BST.
struct address_t* find(struct address_t* current, char* alias);

//lookup the associated address for this alias
int lookup(struct address_t* current, char* alias);
void update_address(char* alias);//Change the associated address for this alias
void delete_address(char* alias);//Delete address/alias pair from the list
void display_aliases(int first, int second);//Show aliases that start with x.y
void save(char* filename);//save the list to file
void write(struct address_t* current, FILE* fp);//Helper function to traverse BST.
int print_menu();//helper function to redisplay the menu for the user.
void print_nodes(struct address_t* current);//display all nodes
void removeNewline(char* str);//helper function, strips newlines from input
void cleanup(struct address_t* current);//Frees all nodes.
struct address_t* insert_node(struct address_t* current,struct address_t* node);
//==============================================================================

int main()
{

  read_in_data();

  //print_nodes(head->next);

  char buffer[100];

  int flag = 1;
  while(flag)
  {
    int choice = print_menu();
    char address[100];
    char alias[100];

    int first = -1; //Initialize for option 6
    int second = -1;//Initialize for option 6
    int* params[2]; //Initialize for option 6

    switch(choice)
    {
//==============================================================================
      case 1://add address
      printf("Please enter an address: ");
      if(!(fgets(buffer, sizeof(buffer), stdin))) //Read in first line of file.
      {
          perror("Failed to read in string. Exiting.\n");
          cleanup(root);
          exit(-1);
      }

      memcpy(address, buffer, sizeof(address));

      printf("Please enter an alias: ");
      if(!(fgets(buffer, sizeof(buffer), stdin))) //Read in first line of file.
      {
          perror("Failed to read in string. Exiting.\n");
          cleanup(root);
          exit(-1);
      }

      memcpy(alias, buffer, sizeof(alias));
      add_address(address,alias);
      break;
//==============================================================================
      case 2://lookup address
      printf("Enter alias: ");
      if(!(fgets(buffer, sizeof(buffer), stdin))) //Read in first line of file.
      {
          perror("Failed to read in string. Exiting.\n");
          cleanup(root);
          exit(-1);
      }
      memcpy(alias, buffer, sizeof(alias));
      removeNewline(alias);
      if(lookup(root,alias) == 0)
      {
        printf("%s does not exist.\n", alias);
      }
      break;
//==============================================================================
      case 3://update address
      printf("Enter alias: ");
      if(!(fgets(buffer, sizeof(buffer), stdin))) //Read in first line of file.
      {
          perror("Failed to read in string. Exiting.\n");
          cleanup(root);
          exit(-1);
      }
      memcpy(alias, buffer, sizeof(alias));
      removeNewline(alias);
      update_address(alias);
      break;
//==============================================================================
      case 4://delete address
      printf("Enter alias: ");
      if(!(fgets(buffer, sizeof(buffer), stdin))) //Read in first line of file.
      {
          perror("Failed to read in string. Exiting.\n");
          cleanup(root);
          exit(-1);
      }
      memcpy(alias, buffer, sizeof(alias));
      removeNewline(alias);
      delete_address(alias);
      break;
//==============================================================================
      case 5://display list
      print_nodes(root);
      break;
//==============================================================================
      case 6://display aliases for location
      params[0] = &first;
      params[1] = &second;

      int i;
      for(i=0; i<2; i++)
      {
        int* temp = (int*) params[i];

        while((*temp < 0) || (*temp > 255))
        {
          printf("Enter location value #%d (0-255): ",i+1);
          if(!(fgets(buffer, sizeof(buffer), stdin)))
          {
              perror("Failed to read in string. Exiting.\n");
              cleanup(root);
              exit(-1);
          }

          removeNewline(buffer);
          *temp = atoi(buffer);
          if(!(*temp >= 0) || !(*temp <= 255))
          {
            printf("Illegal entry.\n");
          }
        }
      }

      display_aliases(first, second);
      break;
//==============================================================================
      case 7://save to file
      printf("Enter filename: ");
      if(!(fgets(buffer, sizeof(buffer), stdin)))
      {
          perror("Failed to read in string. Exiting.\n");
          cleanup(root);
          exit(-1);
      }
      removeNewline(buffer);
      save(buffer);
      break;
//==============================================================================
      case 8://quit
              printf("Goodbye.\n");
              cleanup(root);
              flag = 0;
      break;
//==============================================================================
      default: //Not an option reprompt user.
      printf("Invalid option, enter a number 1-8 to make a selection.\n");
      break;
//==============================================================================
    }
  }
  return 0;
}

int print_menu()
{
  printf("\n");
  printf("1) Add address\n");
  printf("2) Lookup address\n");
  printf("3) Update address\n");
  printf("4) Delete address\n");
  printf("5) Display list\n");
  printf("6) Display aliases for function\n");
  printf("7) Save to file\n");
  printf("8) Quit\n");
  char buffer[30];

  printf("Please enter a number 1-8 to make a selection.\n\n");
  printf("Enter option: ");
  if(!(fgets(buffer, sizeof(buffer), stdin)))
  {
      printf("Failed to read in string. Exiting.\n");
      return 8;
  }

  int choice = atoi(buffer);
  return choice;
}

void read_in_data()
{
  char filename[] = "CS531_Inet.txt";
  FILE *fp;
  char line_buffer[30];
  ssize_t size = 0;

  fp = fopen(filename, "r");

  if(!fp)
  {
    perror("Error while trying to read in data.");
    cleanup(root);
    exit(-1);
  }

  int line = 0;
  int first = 0;
  int second = 0;
  int third = 0;
  int fourth = 0;
  char alias[11];

  //Read in first line of file.
  if(!(fgets(line_buffer, sizeof(line_buffer), fp)))
  {
      perror("Failed to read in string. Exiting.\n");
      cleanup(root);
      exit(-1);
  }
  //process first line
  sscanf(line_buffer,"%d.%d.%d.%d %s", &first, &second, &third, &fourth, alias);

  //define root node
  struct address_t* t = (struct address_t*) malloc(sizeof(struct address_t));
  if(t == NULL)
  {
    perror("Cannot allocate space for new struct exiting.");
    cleanup(root);
    exit(-1);
  }
  t -> first = first;
  t -> second = second;
  t -> third = third;
  t -> fourth = fourth;
  memcpy(t->alias, alias, sizeof(t->alias));
  t -> right = NULL;
  t -> left = NULL;
  root = t;

  //process the rest of the lines
  while(fgets(line_buffer, sizeof(line_buffer), fp))
  {
    sscanf(line_buffer,"%d.%d.%d.%d %s",&first,&second, &third, &fourth, alias);
    struct address_t* temp =(struct address_t*)malloc(sizeof(struct address_t));
    if(temp == NULL)
    {
      perror("Cannot allocate space for new struct exiting.");
      cleanup(root);
      exit(-1);
    }

    temp -> first = first;
    temp -> second = second;
    temp -> third = third;
    temp -> fourth = fourth;
    memcpy(temp->alias, alias, sizeof(temp->alias));
    temp -> right = NULL;
    temp -> left = NULL;

    root = insert_node(root,temp);
  }
  fclose(fp);
}

struct address_t* insert_node(struct address_t* current, struct address_t* node)
{
  if(current == NULL) // Tree is empty
  {
    return node;
  }

  if(strcmp(node -> alias, current -> alias) < 0) //Traverse tree
  {
    current -> left = insert_node(current->left,node);
  }
  else
  {
    current -> right = insert_node(current->right,node);
  }

  return current;
}

void print_nodes(struct address_t* current)
{
  if(current == NULL)
  {
    return;
  }
  print_nodes(current -> left);
  printf("%d.%d.%d.%d %s\n", current->first, current->second, current->third, current->fourth, current -> alias);
  print_nodes(current -> right);
}

void add_address(char* address, char* alias)
{

  int first = 0;
  int second = 0;
  int third = 0;
  int fourth = 0;
  sscanf(address, "%d.%d.%d.%d", &first, &second, &third, &fourth);
  removeNewline(alias);

  if(alias_exists(root,alias))
  {
    printf("%s already exists.\n", alias);
    return;
  }

  if(address_exists(root,first,second,third,fourth))
  {
    printf("%d.%d.%d.%d already exists.\n", first, second, third, fourth);
    return;
  }

  if(!(first >= 0 && first <=255) || !(second >= 0 && second <=255) || !(third >= 0 && third <=255) || !(fourth >= 0 && fourth <=255))
  {
    printf("%d.%d.%d.%d is invalid all sections must be (0-255).\n", first, second, third, fourth);
    return;
  }

  struct address_t* node = (struct address_t*) malloc(sizeof(struct address_t));

  node -> first = first;
  node -> second = second;
  node -> third = third;
  node -> fourth = fourth;
  memcpy(node->alias, alias, sizeof(node->alias));
  node -> left = NULL;
  node -> right = NULL;
  root = insert_node(root, node);

}

//Strips newline characters.
void removeNewline(char* str)
{
  char* position;
  char* pos;

  if((pos = strchr(str, '\n')) != NULL)
  {
    *pos = '\0';
  }
}

int alias_exists(struct address_t* temp, char* alias)
{
  if(temp == NULL)
  {
    return 0;
  }
  else
  {
    if(strcmp(temp -> alias, alias) == 0)
    {
      return 1;
    }
    else
    {
      int left = alias_exists(temp->left, alias);
      int right = alias_exists(temp->right, alias);
      if(left == 1 || right == 1)
      {
        return 1;
      }
      else
      {
        return 0;
      }
    }
  }
}

int address_exists(struct address_t* temp, int first, int second, int third, int fourth)
{
  if(temp == NULL)
  {
    return 0;
  }
  else
  {
    if((temp->first == first) && (temp->second == second) && (temp->third == third) && (temp->fourth == fourth))
    {
      return 1;
    }
    else
    {
      int left = address_exists(temp->left, first, second, third, fourth);
      int right = address_exists(temp->right, first, second, third, fourth);
      if(left == 1 || right == 1)
      {
        return 1;
      }
      else
      {
        return 0;
      }
    }
  }
}

int lookup(struct address_t* current, char* alias)
{
  if(current = NULL)
  {
    return 0;
  }
  else
  {
    if(strcmp(current->alias, alias) == 0)
    {
      printf("Address for %s: %d.%d.%d.%d\n", alias, current->first, current->second, current->third, current->fourth);
      return 1;
    }
    else
    {
      int left = lookup(current -> left, alias);
      int right = lookup(current -> right, alias);
      if(left == 1 || right == 1)
      {
        return 1;
      }
      else
      {
        return 0;
      }
    }
  }

}

struct address_t* find(struct address_t* current, char* alias)
{
  if(current = NULL)
  {
    return NULL;
  }
  else
  {
    if(strcmp(current->alias, alias) == 0)
    {
      printf("Update Address for %s: %d.%d.%d.%d\n", alias, current->first, current->second, current->third, current->fourth);
      return current;
    }
    else
    {
      struct address_t* left = find(current -> left, alias);
      struct address_t* right = find(current -> right, alias);
      if(left != NULL)
      {
        return left;
      }
      else if(right != NULL)
      {
        return right;
      }
      else
      {
        return NULL;
      }
    }
  }
}


void update_address(char* alias)
{
  struct address_t* edit = find(root, alias);
  if(edit == NULL)
  {
    printf("%s does not exist.\n", alias);
    return;
  }

  char buffer[20];
  int first = -1;
  int second = -1;
  int third = -1;
  int fourth = -1;

  int* params[4];
  params[0] = &first;
  params[1] = &second;
  params[2] = &third;
  params[3] = &fourth;


  int i;
  for(i=0; i<4; i++)
  {
    int* temp = (int*) params[i];

    while((*temp < 0) || (*temp > 255))
    {
      printf("Enter location value #%d (0-255): ",i+1);
      if(!(fgets(buffer, sizeof(buffer), stdin)))
      {
          perror("Failed to read in string. Exiting.\n");
          cleanup(root);
          exit(-1);
      }

      removeNewline(buffer);
      *temp = atoi(buffer);
      if(!(*temp >= 0) || !(*temp <= 255))
      {
        printf("Illegal entry.\n");
      }
    }
  }
  //printf("%d.%d.%d.%d\n", first, second, third, fourth);
  if(!address_exists(root,first,second,third,fourth))
  {
    edit -> first = first;
    edit -> second = second;
    edit -> third = third;
    edit -> fourth = fourth;
    //print_nodes(head->next);
  }
  else
  {
    printf("Address already exists, discarding.\n");
    return;
  }
}

void delete_address(char* alias)
{
  struct address_t* temp = head;
  char buffer[20];

  while(temp -> next)
  {
    if(strcmp(temp -> next -> alias, alias) == 0)
    {
      printf("Delete %s %d.%d.%d.%d? (y/n)\n", alias, temp->next->first, temp->next->second, temp->next->third, temp->next->fourth);

      if(!(fgets(buffer, sizeof(buffer), stdin)))
      {
          perror("Failed to read in string. Exiting.\n");
          cleanup(root);
          exit(-1);
      }
      removeNewline(buffer);
      if(strcmp("y",buffer) == 0)
      {
        struct address_t* t = temp->next;
        if(temp -> next -> next == NULL)
        {
          temp -> next = NULL;
        }
        else
        {
          temp -> next = temp -> next -> next;
        }
        free(t);
        printf("%s deleted.\n", alias);
        return;
      }
    }
    temp = temp->next;
  }
  printf("%s does not exist.\n", alias);
}

void display_aliases(int first, int second)
{
  struct address_t* temp = head->next;

  int flag = 0;

  while(temp)
  {
    if(temp -> first == first && temp -> second == second)
    {
      printf("%d.%d.%d.%d %s\n", temp->first, temp->second, temp->third, temp->fourth, temp -> alias);
      flag = 1;
    }
    temp = temp->next;
  }
  if(!flag)
  {
    printf("No locations associated with %d.%d\n", first,second);
  }
}

int display_helper(struct address_t* current)
{
  if(current == NULL)
  {
    return 0;
  }
  else
  {
    if(current -> first == first && current -> second == second)
    {
      printf("%d.%d.%d.%d %s\n", temp->first, temp->second, temp->third, temp->fourth, temp -> alias);
    }
    int left = display_helper(current);
    int right = display_helper(current);
    if(right == 1 || left == 1)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

void save(char* filename)
{

  FILE *fp;
  fp = fopen(filename, "w+");

  if(!fp)
  {
    perror("Could not open file.\n");
    cleanup(root);
    exit(-1);
  }

  write(root, fp);

  fclose(fp);
  printf("File saved. \n");
}

void write(struct address_t* current, FILE* fp)
{
  if(current == NULL)
  {
    return;
  }
  else
  {
    write(current -> left, fp);
    fprintf(fp,"%d.%d.%d.%d %s\n", current->first, current->second, current->third, current->fourth, current -> alias);
    write(current -> left, fp);
  }
}

void cleanup(struct address_t* current)
{
  if(current == NULL)
  {
    return;
  }
  else
  {
    cleanup(current -> left);
    cleanup(current -> right);
    free(current);
  }
}
