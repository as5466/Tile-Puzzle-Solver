#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
	int *board;
	int move;
	struct node *parent;
	struct node *next;
} Node;

typedef struct possib {
	int **array;
	int count;
} Possible;

typedef struct processed {
	int **array;
	int size;
	int capacity;
} Processed;

typedef struct queue {
	Node *head;
	Node *tail;
	int size;
} Queue;

int *swap(int *array, int index1, int index2, int k) {
	int *temp_array = malloc(sizeof(int)*k*k);
	for (int i=0;i<k*k;i++) {
		temp_array[i] = array[i];
	}
	int temp = array[index1];
	temp_array[index1] = temp_array[index2];
	temp_array[index2] = temp;

	return temp_array;
}

int convertToIndex(int i, int j, int k) {
	return ((i*k)+j);
}

Node *initializeNode(int *board, int k) {
	Node *myNode = malloc(sizeof(Node));
	myNode->board = malloc(sizeof(int)*k*k);
	myNode->board = board;
	myNode->next = NULL;
	myNode->parent = NULL;
	myNode->move = -1;
	return myNode;
}

Processed *initializeProcessed(int k) {
	Processed *proc = malloc(sizeof(Processed));
	proc->capacity  = 2;
	proc->array = malloc(sizeof(int *)*2);
	proc->array[0] = malloc(sizeof(int)*k*k);
	proc->array[1] = malloc(sizeof(int)*k*k);
	proc->size = 0;
}

Queue *initializeQueue(int k) {
	Queue *queue = malloc(sizeof(Queue));
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
}

void insertProcessed(Processed *proc, int k, int *board) {
	proc->size++;
	if (proc->size == proc->capacity) {
		proc->capacity = 2*proc->capacity;
		proc->array = realloc(proc->array, sizeof(int *)*proc->capacity);
		for (int i = proc->size-1;i<proc->capacity;i++) {
			proc->array[i] = malloc(sizeof(int)*k*k);
		}
	
	}
	proc->array[proc->size-1] = board;	
}

void Enqueue(Queue *queue, Node *board) {
	if (queue->size == 0) {
		queue->size++;
		queue->head = board;
		return;
	}
	Node *temp = queue->head;
	while (temp->next != NULL) {
		temp = temp->next;
	}
	temp->next = board;
	queue->size++;
	return;
}

Node *Dequeue(Queue *queue, int k) {
	Node *temp = queue->head;
	queue->head = queue->head->next;
	queue->size--;
	return temp;

}

Possible *possibleIJ(int zeroIndex, int k) {
	Possible *possible = malloc(sizeof(Possible));
	possible->array = malloc(sizeof(int *));
	possible->count = 0;

	int i = zeroIndex/k;
	int j = zeroIndex % k;

	if (j+1 < k) {
		possible->count++;

		possible->array = realloc(possible->array, sizeof(int *)*possible->count);
		possible->array[possible->count-1] = malloc(sizeof(int)*2);
		possible->array[possible->count-1][0] = i;
		possible->array[possible->count-1][1] = j+1;

	}

	if (j-1 > -1) {
		possible->count++;
		possible->array = realloc(possible->array, sizeof(int *)*possible->count);

		possible->array[possible->count-1] = malloc(sizeof(int)*2);
		possible->array[possible->count-1][0] = i;
		possible->array[possible->count-1][1] = j-1;

	}

	if (i+1 < k) {
		possible->count++;
		possible->array = realloc(possible->array, sizeof(int *)*possible->count);
		possible->array[possible->count-1] = malloc(sizeof(int)*2);
		possible->array[possible->count-1][0] = i+1;
		possible->array[possible->count-1][1] = j;

	}

	if (i-1 > -1) {
		possible->count++;
		possible->array = realloc(possible->array, sizeof(int *)*possible->count);
		possible->array[possible->count-1] = malloc(sizeof(int)*2);
		possible->array[possible->count-1][0] = i-1;
		possible->array[possible->count-1][1] = j;

	}

	return possible;
}

Node *BFS(Node *initial_board_Node, int k, int *goal_board) {
	int zero_index;
	Node *noSoln = initializeNode(NULL, 0);
	for (int i=0;i<k*k;i++) {
			if (initial_board_Node->board[i] == 0) {
				zero_index = i;
			}
	}

	int inv_count = 0;

	for (int i=0;i<k*k-1;i++) {
		for (int j=i+1;j<k*k;j++) {
			if (initial_board_Node->board[i] != 0 && initial_board_Node->board[j] != 0 && initial_board_Node->board[i] > initial_board_Node->board[j]) {
				inv_count++;
			}
		}
	}
	int solvable = -1;

	if (k%2 == 1) {
		if (inv_count%2 == 0) {
			solvable = 1;
		}
	}
	else {
		if (((k-zero_index/k) % 2 == 0) && (inv_count%2 == 1)) {
			solvable = 1;
		}
		else if (((k-zero_index/k)%2 == 1) && (inv_count%2 == 0)) {
			solvable = 1;
		}
	}

	if (solvable == -1) {
		return noSoln;
	}
	

	
	Queue *queue = initializeQueue(k);
	Enqueue(queue, initial_board_Node);
	Processed *proc = initializeProcessed(k);
	int valid = 1;
	int temp_index;
	int exists = 1;
	int double_break = -1;

	while (queue->head != NULL) {

		Node *dequeue = Dequeue(queue, k);

		for (int i=0;i<k*k;i++) {
			if (dequeue->board[i] != goal_board[i]) {
				valid = -1;
			}
		}
		if (valid == 1) {
			return dequeue;
		}
		
		insertProcessed(proc, k, dequeue->board);	


		for (int i=0;i<k*k;i++) {
			if (dequeue->board[i] == 0) {
				zero_index = i;
			}
		}
	
		Possible *possible = possibleIJ(zero_index, k);

		for (int i=0;i<possible->count;i++) {
			temp_index = convertToIndex(possible->array[i][0], possible->array[i][1], k);
			Node *newBoard = initializeNode(swap(dequeue->board, temp_index, zero_index, k), k);

			for (int j=0; j<proc->size;j++) {
				exists = 1;
				for(int l=0; l<k*k;l++) {
					
					if (proc->array[j][l] != newBoard->board[l]) {
						exists = -1;
					}
				}

				if (exists == 1) {
					double_break = 1;
					break;
				}	
			}

			Node *parse = queue->head;

			if (double_break != 1 && parse != NULL) {
				exists = 1;
				while (parse != NULL) {
					exists = 1;
					for (int m=0; m<k*k; m++) {
						if (parse->board[m] != newBoard->board[m]) {
							exists = -1;
						}
					}
					if (exists == 1) {
						break;
					}
					parse = parse->next;
					
				}	
			}
			

			if (exists == -1) {

				newBoard->parent = dequeue;
				newBoard->move = dequeue->board[temp_index];
				Enqueue(queue, newBoard);
			}
			
		}		
		valid = 1;
		exists = 1;
		double_break = -1;
		
	}

}


int main(int argc, char **argv)
{
	FILE *fp_in,*fp_out;
	
	fp_in = fopen(argv[1], "r");
	if (fp_in == NULL){
		printf("Could not open a file.\n");
		return -1;
	}
	
	fp_out = fopen(argv[2], "w");
	if (fp_out == NULL){
		printf("Could not open a file.\n");
		return -1;
	}
	
	char *line = NULL;
	size_t lineBuffSize = 0;
	ssize_t lineSize;
	int k;
	
	getline(&line,&lineBuffSize,fp_in);//ignore the first line in file, it is a comment
	fscanf(fp_in,"%d\n",&k);
	getline(&line,&lineBuffSize,fp_in);//ignore the second line in file, it is a comment

	int initial_board[k*k];/
	for(int i=0;i<k*k;i++)
		fscanf(fp_in,"%d ",&initial_board[i]);

	fclose(fp_in);


	int goal_board[k*k];
	for (int i=0;i<k*k-1;i++) {
		goal_board[i] = i+1;
	}

	goal_board[k*k-1] = 0;

	Node *initial_board_node = initializeNode(&initial_board[0], k);
	Node *goal_board_node = initializeNode(&goal_board[0], k);
	Node *solution = BFS(initial_board_node, k, goal_board);

	if (solution->board == NULL) {
		fprintf(fp_out, "#moves\n");
		fprintf(fp_out, "no solution\n");
	}
	else {
		Node *parse = solution;

		int *soln_arr = malloc(sizeof(int));
		int count = 0;

		while (parse!=NULL) {
			if (parse->move > -1) {
				count++;
				soln_arr = realloc(soln_arr, sizeof(int)*count);
				soln_arr[count-1] = parse->move;			
			}
			parse = parse->parent;
		}

		fprintf(fp_out, "#moves\n");

		for (int i=count;i>0;i--) {
			fprintf(fp_out, "%d ", soln_arr[i-1]);
		}
		fprintf(fp_out, "\n");
	}

	fclose(fp_out);


}
