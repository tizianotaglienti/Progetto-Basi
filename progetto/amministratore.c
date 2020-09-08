#include "defines.h"

void show_user_features(){
	MYSQL *conn;
	conn = connection_db();
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];
	
	char cf_user[16];
	
	printf("\nUser's CF: ");
	getInput(16, cf_user, false);
	
	
	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call visualizza_utente(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize user statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;	//IN
	param[0].buffer = cf_user;
	param[0].buffer_length = strlen(cf_user);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for user report\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error(prepared_stmt, "An error occurred while retrieving the user report.");
		goto out;
	}
	
	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nUser's credentials");
	
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

void insert_auction(MYSQL *conn){
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[9];

	
	char cod_object[16];
	int price;
	char size[45];
	char color[45];
	char cond[45];
	char description[100];

	MYSQL_TIME time_durata;
	bzero(&date, sizeof(MYSQL_TIME));			//init struct MYSQL_TIME
	time_durata.time_type = MYSQL_TYPE_TIME;
	
	int category;
		
	//Get parameters
	printf("\nObject code: ");
	getInput(16, cod_object, false);
	printf("\nPrice: \n");
	scanf("%d", &price);
	printf("\nSize: ");
	getInput(45, size, false);
	printf("\nColor: \n");
	getInput(45, color, false);
	printf("\nConditions: ");
	getInput(45, cond, false);
	printf("\nDescription: \n");
	getInput(100, description, false);

	printf("\nDuration:\n");
	printf("\nHours: ");
	scanf("%d", &(time_durata.hour));
	printf("\nMinutes: ");
	scanf("%d", &(time_durata.minute));
	printf("\nSeconds: ");
	scanf("%d", &(time_durata.second));

	printf("\nCategory : ");
	scanf("%d", &category);
		
	
	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call inserisci_proposta_accettata(?, ?, ?, ?, ?, ?, ?, ?,)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize auction insertion statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;	//IN
	param[0].buffer = cod_object;
	param[0].buffer_length = strlen(cod_object);

	param[1].buffer_type = MYSQL_TYPE_LONG;			//IN
	param[1].buffer = &price;
	param[1].buffer_length = sizeof(price);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;	//IN
	param[2].buffer = size;
	param[2].buffer_length = strlen(size);

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;	//IN
	param[3].buffer = color;
	param[3].buffer_length = strlen(color);

	param[4].buffer_type = MYSQL_TYPE_VAR_STRING;	//IN
	param[4].buffer = cond;
	param[4].buffer_length = strlen(cond);

	param[5].buffer_type = MYSQL_TYPE_VAR_STRING;	//IN
	param[5].buffer = description;
	param[5].buffer_length = strlen(description);

	param[6].buffer_type = MYSQL_TYPE_TIME;			//IN
	param[6].buffer = (char *)&time_durata;
	param[6].buffer_length = sizeof(time_durata);
	param[6].is_null=0;

	param[7].buffer_type = MYSQL_TYPE_TIME;			//IN
	param[7].buffer = (char *)&time_missing;
	param[7].buffer_length = sizeof(time_missing);
	param[7].is_null=0;

	param[8].buffer_type = MYSQL_TYPE_VAR_STRING;	//IN
	param[8].buffer = category;
	param[8].buffer_length = sizeof(category);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for auction insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error(prepared_stmt, "An error occurred while inserting the auction.");
		goto out;
	}

	printf("Auction correctly inserted!\n");

    out:
	mysql_stmt_close(prepared_stmt);

}

void object_report(){
	MYSQL *conn;
	conn = connection_db();
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];
	
	char cod_object[16];
	
	printf("\nObject code: ");
	getInput(16, cod_object, false);
	
	
	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call report_oggetto(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize object report statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;	//IN
	param[0].buffer = cod_object;
	param[0].buffer_length = strlen(cod_object);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for object report\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error(prepared_stmt, "An error occurred while retrieving the object report.");
		goto out;
	}
	
	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nObject report");
	
	out:
	mysql_stmt_close(prepared_stmt);
	
}


void start_amministratore_view(MYSQL *conn){
	// Admin main
	char options[5] = {'1','2','3','4','5'};
	char op;
	
	printf("Welcome in the system!\n");

	if(!parse_config("users/amministratore.json", &conf)) {
		fprintf(stderr, "Unable to load client configuration\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}
	printf("\033[2J\033[H");
	while(1) {
		printf("********** ADMIN VIEW **********\n\n");
		printf("*** What should I do for you? ***\n\n");
		printf("1) Show user features\n");
        printf("2) Show auction features\n");
        printf("3) Make a history report for an object\n");
        printf("4) Insert a new auction\n");
		printf("5) Quit\n");

		op = multiChoice("Select an option", options, 5);

		switch(op) {
			case '1':				
				show_user_features();
				break;
            case '2':
				show_auction_features();
                break;
            case '3':
				object_report();
                break;
            case '4':
				insert_auction(conn);
                break;
			case '5':
				return;
		
				
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();
	}

}