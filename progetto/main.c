/*
 * 
 ************** MAIN ***************** 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "defines.h"

typedef enum {
    UTENTE = 1,
    AMMINISTRATORE,
	FAILED_LOGIN
} role_t;

struct configuration 	conf; 		//struct for config of db
char 					cf[16]; 	//codice fiscale of the running user
static MYSQL 			*conn;		//struct fot connection to db 


static role_t attempt_login(MYSQL *conn, char *cf, char *password) {
	MYSQL_STMT *login_procedure;		
	
	MYSQL_BIND param[3]; 				//Used both for input and output
	int role = 0;
	// Prepare the statement avoid injection
	if(!setup_prepared_stmt(&login_procedure, "call login(?, ?, ?)", conn)) {
		print_stmt_error(login_procedure, "Unable to initialize login statement\n");
		goto err2;
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING; 	// IN
	param[0].buffer = cf;
	param[0].buffer_length = strlen(cf);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING; 	// IN
	param[1].buffer = password;
	param[1].buffer_length = strlen(password);

	param[2].buffer_type = MYSQL_TYPE_LONG; 		// OUT
	param[2].buffer = &role;
	param[2].buffer_length = sizeof(role);


	// Bind the parameters with the procedure
	if (mysql_stmt_bind_param(login_procedure, param) != 0) { // Note _param
		print_stmt_error(login_procedure, "Could not bind parameters for login");
		goto err;
	}

	// Run procedure
	if (mysql_stmt_execute(login_procedure) != 0) {
		print_stmt_error(login_procedure, "Could not execute login procedure");
		goto err;
	}

	// Prepare output parameters
	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_LONG; // OUT
	param[0].buffer = &role;
	param[0].buffer_length = sizeof(role);
	
	
	if(mysql_stmt_bind_result(login_procedure, param)) {
		print_stmt_error(login_procedure, "Could not retrieve output parameter");
		goto err;
	}
	
	// Retrieve output parameter
	if(mysql_stmt_fetch(login_procedure)) {
		print_stmt_error(login_procedure, "Could not buffer results");
		goto err;
	}
	
	
	mysql_stmt_close(login_procedure);

	return role;
	
    err:
	mysql_stmt_close(login_procedure);
	
    err2:
	return FAILED_LOGIN;
}

int main(void)
{
	role_t role;
	// Parse the .json file for take info for db config
	if(!parse_config("users/login.json", &conf)) {
		fprintf(stderr, "Unable to load login configuration\n");
		exit(EXIT_FAILURE);
	}

	conn = mysql_init (NULL);
	if (conn == NULL) {
		fprintf (stderr, "mysql_init() failed \n");
		exit(EXIT_FAILURE);
	}
	// Connect with config parameters to the db
	if (mysql_real_connect(conn, conf.host, conf.db_username, conf.db_password, conf.database, conf.port, NULL, CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS) == NULL) {
		fprintf (stderr, "mysql_real_connect() failed\n");
		mysql_close (conn);
		exit(EXIT_FAILURE);
	}
	// Take login data from input
	printf("Codice fiscale: ");
	getInput(16, cf, false);
	printf("Password: ");
	getInput(45, conf.password, true);
	// Switch through the role 
	role = attempt_login(conn, cf, conf.password);
	switch(role) {
		case UTENTE:
			start_utente_view(conn);
			break;
			
		case AMMINISTRATORE:
			start_amministratore_view(conn);
			break;
			
		case FAILED_LOGIN:
			fprintf(stderr, "Invalid credentials\n");
			exit(EXIT_FAILURE);
			break;
		
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}
	printf("Exiting from the app...\n");

	// Close the db connection 
	mysql_close (conn);
	
	return 0;
	
}