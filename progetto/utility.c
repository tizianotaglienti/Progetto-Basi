/*
 * 
 ************** UTILITY ***************** 
 * 	
 * here there're implemented some useful
 * functions for the app	
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

// Per la gestione dei segnali
static volatile sig_atomic_t signo;
typedef struct sigaction sigaction_t;
static void handler(int s);

MYSQL* connection_db(){
	MYSQL *conn;
	conn = mysql_init (NULL);
	if (conn == NULL) {
		fprintf (stderr, "mysql_init() failed \n");
		exit(EXIT_FAILURE);
	}
	
	if (mysql_real_connect(conn, conf.host, conf.db_username, conf.db_password, conf.database, conf.port, NULL, CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS) == NULL) {
		fprintf (stderr, "mysql_real_connect() failed\n");
		mysql_close (conn);
		exit(EXIT_FAILURE);
	}
	return conn;
}


void print_stmt_error (MYSQL_STMT *stmt, char *message)
{
	fprintf (stderr, "%s\n", message);
	if (stmt != NULL) {
		fprintf (stderr, "Error %u (%s): %s\n",
			mysql_stmt_errno (stmt),
			mysql_stmt_sqlstate(stmt),
			mysql_stmt_error (stmt));
	}
}


void print_error(MYSQL *conn, char *message)
{
	fprintf (stderr, "%s\n", message);
	if (conn != NULL) {
		#if MYSQL_VERSION_ID >= 40101
		fprintf (stderr, "Error %u (%s): %s\n",
		mysql_errno (conn), mysql_sqlstate(conn), mysql_error (conn));
		#else
		fprintf (stderr, "Error %u: %s\n",
		mysql_errno (conn), mysql_error (conn));
		#endif
	}
}


char *getInput(unsigned int lung, char *stringa, bool hide)
{
	char c;
	unsigned int i;

	// Dichiara le variabili necessarie ad un possibile mascheramento dell'input
	sigaction_t sa, savealrm, saveint, savehup, savequit, saveterm;
	sigaction_t savetstp, savettin, savettou;
	struct termios term, oterm;
	
	if(hide) {
		// Svuota il buffer
		(void) fflush(stdout);

		// Cattura i segnali che altrimenti potrebbero far terminare il programma, lasciando l'utente senza output sulla shell
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_INTERRUPT; // Per non resettare le system call
		sa.sa_handler = handler;
		(void) sigaction(SIGALRM, &sa, &savealrm);
		(void) sigaction(SIGINT, &sa, &saveint);
		(void) sigaction(SIGHUP, &sa, &savehup);
		(void) sigaction(SIGQUIT, &sa, &savequit);
		(void) sigaction(SIGTERM, &sa, &saveterm);
		(void) sigaction(SIGTSTP, &sa, &savetstp);
		(void) sigaction(SIGTTIN, &sa, &savettin);
		(void) sigaction(SIGTTOU, &sa, &savettou);
	
		// Disattiva l'output su schermo
		if (tcgetattr(fileno(stdin), &oterm) == 0) {
			(void) memcpy(&term, &oterm, sizeof(struct termios));
			term.c_lflag &= ~(ECHO|ECHONL);
			(void) tcsetattr(fileno(stdin), TCSAFLUSH, &term);
		} else {
			(void) memset(&term, 0, sizeof(struct termios));
			(void) memset(&oterm, 0, sizeof(struct termios));
		}
	}
	
	// Acquisisce da tastiera al più lung - 1 caratteri
	for(i = 0; i < lung; i++) {
		
		(void) fread(&c, sizeof(char), 1, stdin);
		if(c == '\n') {
			stringa[i] = '\0';
			break;
		} else
			stringa[i] = c;

		// Gestisce gli asterischi
		if(hide) {
			if(c == '\b') // Backspace
				(void) write(fileno(stdout), &c, sizeof(char));
			else
				(void) write(fileno(stdout), "*", sizeof(char));
		}
		
		
	}

	if( strlen(stringa) == 0){
			printf("Att: non sono consentiti valori nulli!\nInserisci di nuovo: ");
			getInput(lung, stringa, hide);
			return stringa;
		
	}
	
	// Controlla che il terminatore di stringa sia stato inserito
	if(i == lung - 1)
		stringa[i] = '\0';

	// Se sono stati digitati più caratteri, svuota il buffer della tastiera
	if(strlen(stringa) >= lung) {	
		// Svuota il buffer della tastiera
		do {
			c = getchar();
		} while (c != '\n');
	}

	if(hide) {
		//L'a capo dopo l'input
		(void) write(fileno(stdout), "\n", 1);

		// Ripristina le impostazioni precedenti dello schermo
		(void) tcsetattr(fileno(stdin), TCSAFLUSH, &oterm);

		// Ripristina la gestione dei segnali
		(void) sigaction(SIGALRM, &savealrm, NULL);
		(void) sigaction(SIGINT, &saveint, NULL);
		(void) sigaction(SIGHUP, &savehup, NULL);
		(void) sigaction(SIGQUIT, &savequit, NULL);
		(void) sigaction(SIGTERM, &saveterm, NULL);
		(void) sigaction(SIGTSTP, &savetstp, NULL);
		(void) sigaction(SIGTTIN, &savettin, NULL);
		(void) sigaction(SIGTTOU, &savettou, NULL);

		// Se era stato ricevuto un segnale viene rilanciato al processo stesso
		if(signo)
			(void) raise(signo);
	}
	
	return stringa;
}


//Per preparare i prepared statement
bool setup_prepared_stmt(MYSQL_STMT **stmt, char *statement, MYSQL *conn)
{
	_Bool update_length = true;

	*stmt = mysql_stmt_init(conn);
	if (*stmt == NULL)
	{
		print_error(conn, "Could not initialize statement handler");
		return false;
	}

	if (mysql_stmt_prepare (*stmt, statement, strlen(statement)) != 0) {
		print_stmt_error(*stmt, "Could not prepare statement");
		return false;
	}

	mysql_stmt_attr_set(*stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &update_length);

	return true;
}


// Per la gestione dei segnali
static void handler(int s) {
	signo = s;
}


char multiChoice(char *domanda, char choices[], int num)
{

	// Genera la stringa delle possibilità
	char *possib = malloc(2 * num * sizeof(char));
	int i, j = 0;
	for(i = 0; i < num; i++) {
		possib[j++] = choices[i];
		possib[j++] = '/';
	}
	possib[j-1] = '\0'; // Per eliminare l'ultima '/'

	// Chiede la risposta
	while(true) {
		// Mostra la domanda
		printf("%s [%s]: ", domanda, possib);

		char c;
		getInput(1, &c, false);

		// Controlla se è un carattere valido
		for(i = 0; i < num; i++) {
			if(c == choices[i])
				return c;
		}
	}
}


void finish_with_error(MYSQL *conn, char *message)
{
	print_error(conn, message);
	mysql_close(conn);
	exit(EXIT_FAILURE);        
}


void finish_with_stmt_error(MYSQL *conn, MYSQL_STMT *stmt, char *message, bool close_stmt)
{
	print_stmt_error(stmt, message);
	if(close_stmt) 	mysql_stmt_close(stmt);
	mysql_close(conn);
	exit(EXIT_FAILURE);        
}


static void print_dashes(MYSQL_RES *res_set)
{
	MYSQL_FIELD *field;
	unsigned int i, j;

	mysql_field_seek(res_set, 0);
	putchar('+');
	for (i = 0; i < mysql_num_fields(res_set); i++) {
		field = mysql_fetch_field(res_set);
		for (j = 0; j < field->max_length + 2; j++)
			putchar('-');
		putchar('+');
	}
	putchar('\n');
}


static void dump_result_set_header(MYSQL_RES *res_set)
{
	MYSQL_FIELD *field;
	unsigned long col_len;
	unsigned int i;

	/* determine column display widths -- requires result set to be */
	/* generated with mysql_store_result(), not mysql_use_result() */

	mysql_field_seek (res_set, 0);

	for (i = 0; i < mysql_num_fields (res_set); i++) {
		field = mysql_fetch_field (res_set);
		col_len = strlen(field->name);

		if (col_len < field->max_length)
			col_len = field->max_length;
		if (col_len < 4 && !IS_NOT_NULL(field->flags))
			col_len = 4; /* 4 = length of the word "NULL" */
		field->max_length = col_len; /* reset column info */
	}
	
	print_dashes(res_set);
	putchar('|');
	mysql_field_seek (res_set, 0);
	for (i = 0; i < mysql_num_fields(res_set); i++) {
		field = mysql_fetch_field(res_set);
		printf(" %-*s |", (int)field->max_length, field->name);
	}
	putchar('\n');

	print_dashes(res_set);
}


void dump_result_set(MYSQL *conn, MYSQL_STMT *stmt, char *title)
{
	int i;
	int status;
	int num_fields;  	  /* number of columns in result */
	MYSQL_FIELD *fields;  /* for result set metadata */
	MYSQL_BIND *rs_bind;  /* for output buffers */
	MYSQL_RES *rs_metadata;
	MYSQL_TIME *date;
	size_t attr_size;

	/* Prefetch the whole result set. This in conjunction with
	 * STMT_ATTR_UPDATE_MAX_LENGTH set in `setup_prepared_stmt`
	 * updates the result set metadata which are fetched in this
	 * function, to allow to compute the actual max length of
	 * the columns.
	 */
	if (mysql_stmt_store_result(stmt)) {
		fprintf(stderr, " mysql_stmt_execute(), 1 failed\n");
		fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
		exit(0);
	}      

	/* the column count is > 0 if there is a result set */
	/* 0 if the result is only the final status packet */
	num_fields = mysql_stmt_field_count(stmt);

	if (num_fields > 0) {
		/* there is a result set to fetch */
		printf("%s\n", title);

		if((rs_metadata = mysql_stmt_result_metadata(stmt)) == NULL) {
			finish_with_stmt_error(conn, stmt, "Unable to retrieve result metadata\n", true);
		}

		dump_result_set_header(rs_metadata);
	    
		fields = mysql_fetch_fields(rs_metadata);

		rs_bind = (MYSQL_BIND *)malloc(sizeof (MYSQL_BIND) * num_fields);
		if (!rs_bind) {
			finish_with_stmt_error(conn, stmt, "Cannot allocate output buffers\n", true);
		}
		memset(rs_bind, 0, sizeof (MYSQL_BIND) * num_fields);

		/* set up and bind result set output buffers */
		for (i = 0; i < num_fields; ++i) {

			// Properly size the parameter buffer
			switch(fields[i].type) {
				case MYSQL_TYPE_DATE:
				case MYSQL_TYPE_TIMESTAMP:
				case MYSQL_TYPE_TIME:
					attr_size = sizeof(MYSQL_TIME);
					break;
				case MYSQL_TYPE_DATETIME:
					
					attr_size = sizeof(MYSQL_TIME);
					break;
					
				case MYSQL_TYPE_FLOAT:
					attr_size = sizeof(float);
					break;
				case MYSQL_TYPE_DOUBLE:
					attr_size = sizeof(double);
					break;
				case MYSQL_TYPE_TINY:
					attr_size = sizeof(signed char);
					break;
				case MYSQL_TYPE_SHORT:
				case MYSQL_TYPE_YEAR:
					attr_size = sizeof(short int);
					break;
				case MYSQL_TYPE_LONG:
				case MYSQL_TYPE_INT24:
					attr_size = sizeof(int);
					break;
				case MYSQL_TYPE_LONGLONG:
					attr_size = sizeof(int);
					break;
				default:
					attr_size = fields[i].max_length;
					break;
			}
			
			// Setup the binding for the current parameter
			rs_bind[i].buffer_type = fields[i].type;
			rs_bind[i].buffer = malloc(attr_size + 1);
			rs_bind[i].buffer_length = attr_size + 1;

			if(rs_bind[i].buffer == NULL) {
				finish_with_stmt_error(conn, stmt, "Cannot allocate output buffers\n", true);
			}
		}

		if(mysql_stmt_bind_result(stmt, rs_bind)) {
			finish_with_stmt_error(conn, stmt, "Unable to bind output parameters\n", true);
		}

		/* fetch and display result set rows */
		while (true) {
			status = mysql_stmt_fetch(stmt);

			if (status == 1 || status == MYSQL_NO_DATA)
				break;

			putchar('|');

			for (i = 0; i < num_fields; i++) {

				if (rs_bind[i].is_null_value) {
					printf (" %-*s |", (int)fields[i].max_length, "NULL");
					continue;
				}

				switch (rs_bind[i].buffer_type) {
					
					
					case MYSQL_TYPE_DATETIME:
						date = (MYSQL_TIME *)rs_bind[i].buffer;
						
						printf("        %d-%02d-%02d    %02d:%02d:%02d        |", date->year, date->month, date->day, date->hour, date->minute, date->second);

						break;
				       
				    case MYSQL_TYPE_TIME:
						date = (MYSQL_TIME *)rs_bind[i].buffer;
						printf(" %02d:%02d:%02d |", date->hour, date->minute, date->second);
						break;
				      
					case MYSQL_TYPE_DATE:
					case MYSQL_TYPE_TIMESTAMP:
						date = (MYSQL_TIME *)rs_bind[i].buffer;
						printf(" %d-%02d-%02d |", date->year, date->month, date->day);
						break;
				    case MYSQL_TYPE_VAR_STRING:
					case MYSQL_TYPE_STRING:
						printf(" %-*s |", (int)fields[i].max_length, (char *)rs_bind[i].buffer);
						break;
		 
					case MYSQL_TYPE_FLOAT:
					case MYSQL_TYPE_DOUBLE:
						printf(" %.02f |", *(float *)rs_bind[i].buffer);
						break;
		 
					case MYSQL_TYPE_LONG:
					case MYSQL_TYPE_SHORT:
					case MYSQL_TYPE_TINY:
						printf(" %-*d |", (int)fields[i].max_length, *(int *)rs_bind[i].buffer);
						break;
				       
					case MYSQL_TYPE_NEWDECIMAL:
						printf(" %-*.02lf |", (int)fields[i].max_length, *(float*) rs_bind[i].buffer);
						break;
	 
					default:
					    printf("ERROR: Unhandled type (%d)\n", rs_bind[i].buffer_type);
					    abort();
				}
			}
			putchar('\n');
			print_dashes(rs_metadata);
		}

		mysql_free_result(rs_metadata); /* free metadata */

		/* free output buffers */
		for (i = 0; i < num_fields; i++) {
			free(rs_bind[i].buffer);
		}
		free(rs_bind);
	}
}
