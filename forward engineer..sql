-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- -----------------------------------------------------
-- Schema mydb
-- -----------------------------------------------------

-- -----------------------------------------------------
-- Schema mydb
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `mydb` DEFAULT CHARACTER SET utf8 ;
USE `mydb` ;

-- -----------------------------------------------------
-- Table `mydb`.`Utente`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `mydb`.`Utente` (
  `cf` VARCHAR(16) NOT NULL,
  `nome` VARCHAR(45) NOT NULL,
  `cognome` VARCHAR(45) NOT NULL,
  `indirizzo` VARCHAR(45) NULL,
  `citta_nascita` VARCHAR(45) NOT NULL,
  `data_nascita` DATE NOT NULL,
  PRIMARY KEY (`cf`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `mydb`.`Amministratore`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `mydb`.`Amministratore` (
  `cf` VARCHAR(16) NOT NULL,
  `nome` VARCHAR(45) NOT NULL,
  `cognome` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`cf`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `mydb`.`Oggetto`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `mydb`.`Oggetto` (
  `codice` VARCHAR(16) NOT NULL,
  `prezzo` INT NOT NULL,
  `dimensioni` VARCHAR(45) NOT NULL,
  `colore` VARCHAR(45) NOT NULL,
  `condizioni` VARCHAR(45) NOT NULL,
  `descrizione` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`codice`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `mydb`.`Asta`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `mydb`.`Asta` (
  `oggetto` VARCHAR(16) NOT NULL,
  `durata` TIME NOT NULL,
  `num_offerte` INT NOT NULL,
  `tempo_mancante` TIME NOT NULL,
  `offerta_maggiore` INT NOT NULL,
  PRIMARY KEY (`oggetto`),
  CONSTRAINT `fk_Asta_Oggetto`
    FOREIGN KEY (`oggetto`)
    REFERENCES `mydb`.`Oggetto` (`codice`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `mydb`.`Oggetto acquistato`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `mydb`.`Oggetto acquistato` (
  `oggetto` VARCHAR(16) NOT NULL,
  `valore_finale` INT NOT NULL,
  PRIMARY KEY (`oggetto`),
  CONSTRAINT `fk_OggettoAcquistato_Oggetto`
    FOREIGN KEY (`oggetto`)
    REFERENCES `mydb`.`Oggetto` (`codice`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `mydb`.`Offerta`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `mydb`.`Offerta` (
  `utente` VARCHAR(16) NOT NULL,
  `oggetto` VARCHAR(16) NOT NULL,
  `importo` INT NOT NULL,
  `istante` DATETIME NOT NULL,
  PRIMARY KEY (`utente`, `importo`, `oggetto`),
  CONSTRAINT `fk_Offerta_Utente`
    FOREIGN KEY (`utente`)
    REFERENCES `mydb`.`Utente` (`cf`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Offerta_Oggetto`
    FOREIGN KEY (`oggetto`)
    REFERENCES `mydb`.`Oggetto` (`codice`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Offerta_Oggetto_idx` ON `mydb`.`Offerta` (`oggetto` ASC) VISIBLE;


-- -----------------------------------------------------
-- Table `mydb`.`Controfferta`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `mydb`.`Controfferta` (
  `utente` VARCHAR(16) NOT NULL,
  `importo_controfferta` INT NOT NULL,
  `oggetto` VARCHAR(16) NOT NULL,
  `importo` INT NOT NULL,
  PRIMARY KEY (`utente`, `oggetto`, `importo`),
  CONSTRAINT `fk_Controfferta_Utente`
    FOREIGN KEY (`utente`)
    REFERENCES `mydb`.`Utente` (`cf`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Controfferta_Offerta`
    FOREIGN KEY (`importo`)
    REFERENCES `mydb`.`Offerta` (`importo`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Controfferta_Oggetto`
    FOREIGN KEY (`oggetto`)
    REFERENCES `mydb`.`Oggetto` (`codice`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Controfferta_Offerta_idx` ON `mydb`.`Controfferta` (`importo` ASC) VISIBLE;

CREATE INDEX `fk_Controfferta_Oggetto_idx` ON `mydb`.`Controfferta` (`oggetto` ASC) VISIBLE;


-- -----------------------------------------------------
-- Table `mydb`.`Categoria`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `mydb`.`Categoria` (
  `asta` INT NOT NULL,
  `oggetto` VARCHAR(16) NOT NULL,
  `livello` INT NOT NULL,
  PRIMARY KEY (`asta`, `oggetto`),
  CONSTRAINT `fk_Categoria_Asta`
    FOREIGN KEY (`asta`)
    REFERENCES `mydb`.`Asta` (`oggetto`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Categoria_Oggetto`
    FOREIGN KEY (`oggetto`)
    REFERENCES `mydb`.`Oggetto` (`codice`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Categoria_Oggetto_idx` ON `mydb`.`Categoria` (`oggetto` ASC) VISIBLE;


-- -----------------------------------------------------
-- Table `mydb`.`Carta di credito`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `mydb`.`Carta di credito` (
  `utente` VARCHAR(16) NOT NULL,
  `numero` INT NOT NULL,
  `intestatario` VARCHAR(45) NOT NULL,
  `data_scadenza` DATE NOT NULL,
  `CVV` INT NOT NULL,
  PRIMARY KEY (`utente`, `numero`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `mydb`.`Visualizza`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `mydb`.`Visualizza` (
  `utente` VARCHAR(16) NOT NULL,
  `oggetto` VARCHAR(16) NOT NULL,
  PRIMARY KEY (`utente`, `oggetto`),
  CONSTRAINT `fk_Visualizza_Utente`
    FOREIGN KEY (`utente`)
    REFERENCES `mydb`.`Utente` (`cf`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Visualizza_Oggetto`
    FOREIGN KEY (`oggetto`)
    REFERENCES `mydb`.`Oggetto` (`codice`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Visualizza_Oggetto_idx` ON `mydb`.`Visualizza` (`oggetto` ASC) VISIBLE;


-- -----------------------------------------------------
-- Table `mydb`.`Acquisto`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `mydb`.`Acquisto` (
  `utente` VARCHAR(16) NOT NULL,
  `oggetto_acquistato` VARCHAR(16) NOT NULL,
  PRIMARY KEY (`oggetto_acquistato`),
  CONSTRAINT `fk_Acquisto_Utente`
    FOREIGN KEY (`utente`)
    REFERENCES `mydb`.`Utente` (`cf`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Acquisto_OggettoAcquistato`
    FOREIGN KEY (`oggetto_acquistato`)
    REFERENCES `mydb`.`Oggetto acquistato` (`oggetto`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Acquisto_Utente_idx` ON `mydb`.`Acquisto` (`utente` ASC) VISIBLE;


-- -----------------------------------------------------
-- Table `mydb`.`Gestione`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `mydb`.`Gestione` (
  `categoria` INT NOT NULL,
  `amministratore` VARCHAR(16) NOT NULL,
  PRIMARY KEY (`categoria`),
  CONSTRAINT `fk_Gestione_Amministratore`
    FOREIGN KEY (`amministratore`)
    REFERENCES `mydb`.`Amministratore` (`cf`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Gestione_Categoria`
    FOREIGN KEY (`categoria`)
    REFERENCES `mydb`.`Categoria` (`asta`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Gestione_Amministratore_idx` ON `mydb`.`Gestione` (`amministratore` ASC) VISIBLE;


-- -----------------------------------------------------
-- Table `mydb`.`Imposta`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `mydb`.`Imposta` (
  `asta` VARCHAR(16) NOT NULL,
  `amministratore` VARCHAR(16) NOT NULL,
  PRIMARY KEY (`asta`),
  CONSTRAINT `fk_Imposta_Amministratore`
    FOREIGN KEY (`amministratore`)
    REFERENCES `mydb`.`Amministratore` (`cf`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Imposta_Asta`
    FOREIGN KEY (`asta`)
    REFERENCES `mydb`.`Asta` (`oggetto`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Imposta_Amministratore_idx` ON `mydb`.`Imposta` (`amministratore` ASC) VISIBLE;


-- -----------------------------------------------------
-- Table `mydb`.`Inserimento`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `mydb`.`Inserimento` (
  `oggetto` VARCHAR(16) NOT NULL,
  `amministratore` VARCHAR(16) NOT NULL,
  PRIMARY KEY (`oggetto`),
  CONSTRAINT `fk_Inserimento_Amministratore`
    FOREIGN KEY (`amministratore`)
    REFERENCES `mydb`.`Amministratore` (`cf`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Inserimento_Oggetto`
    FOREIGN KEY (`oggetto`)
    REFERENCES `mydb`.`Oggetto` (`codice`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_Inserimento_Amministratore_idx` ON `mydb`.`Inserimento` (`amministratore` ASC) VISIBLE;

USE `mydb` ;

-- -----------------------------------------------------
-- procedure create_user
-- -----------------------------------------------------

DELIMITER $$
USE `mydb`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `create_user`(in cf VARCHAR(16), in pass VARCHAR(45), in ruolo VARCHAR(45))
BEGIN
	insert into Utente VALUES(cf, MD5(pass), ruolo);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_amministratore
-- -----------------------------------------------------

DELIMITER $$
USE `mydb`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `inserisci_amministratore`(in codice_fiscale VARCHAR(16), in nome VARCHAR(45), in cognome VARCHAR(45), in pass VARCHAR(45))
BEGIN
	declare var_admin int;
	
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read committed;
	# evito le letture dirty
	start transaction;
		# transazione per controllare se l'amministratore che vado a inserire nel sistema esiste
		# se non esiste lo aggiungo, altrimenti errore
		select count(*)
		from `Amministratore`
		where `cf` = codice_fiscale
		into var_admin;

		if var_admin > 0 then
			signal sqlstate '45002' set message_text = "The admin is already registered";
		end if;
		# se l'amministratore non esiste, lo creo con i suoi attributi
		call `create_user` (codice_fiscale, pass, 'amministratore');
		insert into `Amministratore` (`cf`, `nome`, `cognome`) values (codice_fiscale, nome, cognome);
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_asta
-- -----------------------------------------------------

DELIMITER $$
USE `mydb`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `inserisci_asta`(in codice_oggetto VARCHAR(16), in prezzo_oggetto int, in dimensioni_oggetto VARCHAR(45), in colore_oggetto VARCHAR(45), in condizioni_oggetto VARCHAR(45), in descrizione_oggetto VARCHAR(100), in durata time, in tempo_mancante time, in livello_categoria int)
BEGIN
	declare exit handler for sqlexception
	begin
		rollback;  
		resignal;  
	end;
	set transaction isolation level read committed;
    	# evito le letture dirty 
    	start transaction;
			insert into `Oggetto` values (codice_oggetto, prezzo_oggetto, dimensioni_oggetto, colore_oggetto, condizioni_oggetto, descrizione_oggetto);
		
			insert into `Asta` (`oggetto`, `durata`, `num_offerte`, `tempo_mancante`, `offerta_maggiore`) values (codice_oggetto, durata, 0, tempo_mancante, 0);

			insert into `Categoria` (`asta`, `livello`) values (codice_oggetto, livello_categoria);

	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_offerta
-- -----------------------------------------------------

DELIMITER $$
USE `mydb`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `inserisci_offerta`(in codice_fiscale_utente VARCHAR(16), in codice_oggetto VARCHAR(16), in importo int, in importo_controfferta int)
BEGIN
	declare exit handler for sqlexception
  	begin
		rollback;  
		resignal;  
	end;
	set transaction isolation level read committed;
  	# evito le letture dirty 
    	start transaction;
		insert into `Offerta`(`utente`, `oggetto`, `importo`) values (codice_fiscale_utente, codice_oggetto, importo);
		if importo_controfferta <> 0 then
			insert into `Controfferta` (`utente`, `oggetto`, `importo`, `importo_controfferta`) values (codice_fiscale_utente, codice_oggetto, importo, importo_controfferta);
		end if;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_utente
-- -----------------------------------------------------

DELIMITER $$
USE `mydb`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `inserisci_utente`(in codice_fiscale VARCHAR(16), in nome VARCHAR(45), in cognome VARCHAR(45),  in indirizzo VARCHAR(45), in data_nascita date, in citta_nascita VARCHAR(45))
BEGIN
	declare var_utente int;
	
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;

	set transaction isolation level read committed;
	# evito le letture dirty
	start transaction;
		# transazione per controllare se l'utente che vado a inserire nel sistema esiste
		# se non esiste lo aggiungo, altrimenti errore
		select count(*)
		from `Utente`
		where `cf` = codice_fiscale
		into var_utente;

		if var_utente > 0 then
			signal sqlstate '45001' set message_text = "The user is already registered";
		end if;
		# se l'utente non esiste, lo creo con i suoi attributi
		call `create_user` (codice_fiscale, codice_fiscale, 'utente');
		insert into `Utente` (`cf`, `nome`, `cognome`, `indirizzo`, `dataNascita`, `cittaNascita`) values (codice_fiscale, nome, cognome, indirizzo, data_nascita, citta_nascita);
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure login
-- -----------------------------------------------------

DELIMITER $$
USE `mydb`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `login`(in codice_fiscale varchar(16), in pass varchar(45), out var_role INT)
BEGIN
	declare var_user_role ENUM('utente', 'amministratore');
    
    	select u.`ruolo` into var_user_role
		from `Utente` as u
		where u.`cf` = codice_fiscale
		and u.`password` = md5(pass);
	    
		if var_user_role = 'utente' then
			set var_role = 1;
		elseif var_user_role = 'amministratore' then
			set var_role = 2;
		else
			set var_role = 3;
		end if;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure lista_oggetti
-- -----------------------------------------------------

DELIMITER $$
USE `mydb`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `lista_oggetti` (in codice_oggetto VARCHAR(16), in codice_fiscale VARCHAR(16))
BEGIN
	declare var_num_offerte int;
	# un utente vede gli oggetti per cui ha fatto almeno un'offerta
	set transaction read only;
	set transaction isolation level read committed;
		# prendo oggetti from offerta join oggetto join utente
		# where var_num_offerte > 0
		
		select count(*)
		from `Offerta` join `Utente` on `utente` = `cf`
		where `oggetto` = codice_oggetto
		into var_num_offerte;
		
		if var_num_offerte > 0 then
			select `codice`, `descrizione`, `prezzo`, `dimensioni`, `colore`, `condizioni`
			from `Offerta` join `Oggetto` on `oggetto` = `codice` join `Utente` on `utente` = `cf`
			where `cf` = codice_fiscale
			group by `oggetto`;
		end if;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure visualizza_asta
-- -----------------------------------------------------

DELIMITER $$
USE `mydb`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `visualizza_asta` (in codice_oggetto VARCHAR(16))
BEGIN
	# asta visualizzata: utente ottiene
	# tempo mancante, numero offerte, valore attuale
	select a.`tempo_mancante`, a.`num_offerte`, a.`offerta_maggiore`, a.`oggetto`
	from `Asta` as a join `Oggetto` as o on a.`oggetto` = o.`codice`
	where a.`oggetto` = codice_oggetto;

END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure visualizza_utente
-- -----------------------------------------------------

DELIMITER $$
USE `mydb`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `visualizza_utente`(in codice_fiscale VARCHAR(16))
BEGIN
	declare exit handler for sqlexception
	begin
		rollback;
		resignal;
	end;
	
	set transaction read only;
	set transaction isolation level repeatable read;
	# evito letture dirty e letture non ripetibili
	# se cambia un dato vengo aggiornato
		
		select utente.`cf`, utente.`nome`, utente.`cognome`, utente.`indirizzo`, utente.`dataNascita`, utente.`cittaNascita`
		from `Utente`
		where utente.`cf` = codice_fiscale;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure report_oggetto
-- -----------------------------------------------------

DELIMITER $$
USE `mydb`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `report_oggetto`(in codice_oggetto varchar(16))
BEGIN
	declare exit handler for sqlexception
	begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level serializable;
    start transaction;
		select `utente`, `importo`, `istante`, `oggetto`
		from Offerta
		where `oggetto` = codice_oggetto;
	commit;
END$$

DELIMITER ;
CREATE USER 'Utente' IDENTIFIED BY 'utente';

CREATE USER 'Amministratore' IDENTIFIED BY 'amministratore';

GRANT EXECUTE ON procedure `mydb`.`inserisci_asta` TO 'Amministratore';
GRANT EXECUTE ON procedure `mydb`.`report_oggetto` TO 'Amministratore';
GRANT EXECUTE ON procedure `mydb`.`visualizza_asta` TO 'Amministratore';
GRANT EXECUTE ON procedure `mydb`.`visualizza_utente` TO 'Amministratore';
CREATE USER 'Login' IDENTIFIED BY 'login';

GRANT EXECUTE ON procedure `mydb`.`login` TO 'Login';
CREATE USER 'Gestore' IDENTIFIED BY 'gestore';

GRANT EXECUTE ON procedure `mydb`.`create_user` TO 'Gestore';
GRANT EXECUTE ON procedure `mydb`.`inserisci_amministratore` TO 'Gestore';
GRANT EXECUTE ON procedure `mydb`.`inserisci_utente` TO 'Gestore';

SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;

-- -----------------------------------------------------
-- Data for table `mydb`.`Utente`
-- -----------------------------------------------------
START TRANSACTION;
USE `mydb`;
INSERT INTO `mydb`.`Utente` (`cf`, `nome`, `cognome`, `indirizzo`, `citta_nascita`, `data_nascita`) VALUES ('cfU', 'tiziano', 'taglienti', 'via dei matti 0', 'roma', '1999-04-26');

COMMIT;


-- -----------------------------------------------------
-- Data for table `mydb`.`Amministratore`
-- -----------------------------------------------------
START TRANSACTION;
USE `mydb`;
INSERT INTO `mydb`.`Amministratore` (`cf`, `nome`, `cognome`) VALUES ('TGLTZN99D26H501X', 'Tiziano', 'Taglienti');
INSERT INTO `mydb`.`Amministratore` (`cf`, `nome`, `cognome`) VALUES ('cf', 'nome', 'cognome');

COMMIT;


-- -----------------------------------------------------
-- Data for table `mydb`.`Oggetto`
-- -----------------------------------------------------
START TRANSACTION;
USE `mydb`;
INSERT INTO `mydb`.`Oggetto` (`codice`, `prezzo`, `dimensioni`, `colore`, `condizioni`, `descrizione`) VALUES ('12', 10, '40x40x100', 'nero', 'buone', 'è una sedia da assemblare');
INSERT INTO `mydb`.`Oggetto` (`codice`, `prezzo`, `dimensioni`, `colore`, `condizioni`, `descrizione`) VALUES ('5', 2, '10 centimetri', 'giallo', 'nuovo', 'evidenziatore');

COMMIT;


-- -----------------------------------------------------
-- Data for table `mydb`.`Offerta`
-- -----------------------------------------------------
START TRANSACTION;
USE `mydb`;
INSERT INTO `mydb`.`Offerta` (`utente`, `oggetto`, `importo`, `istante`) VALUES ('cfU', '12', 2, '2020-09-06 16:17:01');

COMMIT;


-- -----------------------------------------------------
-- Data for table `mydb`.`Inserimento`
-- -----------------------------------------------------
START TRANSACTION;
USE `mydb`;
INSERT INTO `mydb`.`Inserimento` (`oggetto`, `amministratore`) VALUES ('12', 'cf');

COMMIT;

