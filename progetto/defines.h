/*
 * 
 ************** DEFINES ***************** 
 * 
 */
#pragma once

#include <mysql.h>

struct configuration{
	char *host;
	char *db_username;
	char *db_password;
	unsigned int port;
	char *database;	
	char password[45];
};
extern char 					cf[16];		/*codice fiscale of the current user*/
extern struct configuration 	conf;		


extern MYSQL* 		connection_db();														/*provide connection to the db*/
extern int 			parse_config(char *path, struct configuration *conf);					/*parse a .json, take username, pass, port, db*/
extern char*		getInput(unsigned int lung, char *stringa, bool hide);					/*provide a safe scan of strings with upperbound lung  */
extern char 		multiChoice(char *domanda, char choices[], int num);					/*provide the choice in the gui*/
extern bool 		setup_prepared_stmt(MYSQL_STMT **stmt, char *statement, MYSQL *conn); 	/*setup MYSQL_STMT struct*/
extern void 		print_error (MYSQL *conn, char *message);								/*print error message for the connection  */
extern void 		print_stmt_error (MYSQL_STMT *stmt, char *message);						/*print error message for the statement  */
extern void 		dump_result_set(MYSQL *conn, MYSQL_STMT *stmt, char *title);  			/*print with the right measure the output of a select call*/
extern void 		finish_with_error(MYSQL *conn, char *message);							/*exit the process after a error in the connection   */
extern void 		finish_with_stmt_error(MYSQL *conn, MYSQL_STMT *stmt, char *message, bool close_stmt);		/*exit the process after a error in the statement   */
extern void 		start_utente_view(MYSQL *conn);			/* run with utente's privileges */
extern void 		start_amministratore_view(MYSQL *conn);		/* run with amministratore's privileges */
