#include "defines.h"

void make_offer(MYSQL *conn){
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[5];

	// user's cf already saved in main

	char cod_object[16];
	int amount;
	int c_amount;

	char controfferta_str = "offerta generata da un utente";
	// controfferta_str (enum)

	//Get parameters
	printf("\nObject code: ");
	getInput(16, cod_object, false);
	printf("\nAmount: \n");
	scanf("%d", &amount);
	Label:
	printf("\nCounter offer amount: ");
	scanf("%d", &c_amount);

	if (c_amount < amount + 1 && c_amount != 0){
		printf("Counter offer amount must be greater than the amount plus €1 !\n");
		goto Label;
	}
		
	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call inserisci_proposta_accettata(?, ?, ?, ?, ?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize auction insertion statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;	//IN
	param[0].buffer = cf;
	param[0].buffer_length = strlen(cf);
	
	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;	//IN
	param[1].buffer = cod_object;
	param[1].buffer_length = strlen(cod_object);

	param[2].buffer_type = MYSQL_TYPE_VAR_LONG;	//IN
	param[2].buffer = amount;
	param[2].buffer_length = sizeof(amount);

	param[3].buffer_type = MYSQL_TYPE_VAR_LONG;	//IN
	param[3].buffer = c_amount;
	param[3].buffer_length = sizeof(c_amount);

	param[4].buffer_type = MYSQL_TYPE_VAR_STRING;	//IN
	param[4].buffer = controfferta_str;
	param[4].buffer_length = strlen(controfferta_str);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for offer insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error(prepared_stmt, "An error occurred while inserting the offer.");
		goto out;
	}

	printf("Offer correctly inserted!\n");

    out:
	mysql_stmt_close(prepared_stmt);

}

void show_auction_features(){
	MYSQL *conn;
	conn = connection_db();
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];
	
	char cod_object[16];
	
	printf("\nObject code: ");
	getInput(16, cod_object, false);
	
	
	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call visualizza_asta(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize auction statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;	//IN
	param[0].buffer = cod_object;
	param[0].buffer_length = strlen(cod_object);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for auction report\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error(prepared_stmt, "An error occurred while retrieving the auction report.");
		goto out;
	}
	
	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nAuction features");
	
	out:
	mysql_stmt_close(prepared_stmt);
	
}

void show_obj_list(){
	MYSQL *conn;
	conn = connection_db();
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];
	
	// user's cf already saved in main

	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call lista_oggeti(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize object list statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;	//IN
	param[0].buffer = cf;
	param[0].buffer_length = strlen(cf);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for object list report\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error(prepared_stmt, "An error occurred while retrieving the object list report.");
		goto out;
	}
	
	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nObject list");
	
	out:
	mysql_stmt_close(prepared_stmt);
	
}

void start_utente_view(MYSQL *conn){
	// User main
	char options[4] = {'1','2','3','4'};
	char op;
	
	printf("Welcome in the system!\n");

	if(!parse_config("users/utente.json", &conf)) {
		fprintf(stderr, "Unable to load user configuration\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}
	printf("\033[2J\033[H");
	while(1) {
		printf("********** USER VIEW **********\n\n");
		printf("*** What should I do for you? ***\n\n");
		printf("1) Make an offer\n");
        printf("2) Show an auction\n");
        printf("3) Display the list of interested objects\n");
		printf("4) Quit\n");

		op = multiChoice("Select an option", options, 4);

		switch(op) {
			case '1':				
				make_offer();
				break;
            case '2':
				show_auction_features();
                break;
            case '3':
				show_obj_list();
                break;
			case '4':
				return;
		
				
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();
	}

}