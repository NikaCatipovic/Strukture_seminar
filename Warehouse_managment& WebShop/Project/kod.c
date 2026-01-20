#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h> 
#define MAX_NAME_LEN (32)

struct _article;
typedef struct _article* articleP;
typedef struct _article {
	char name[MAX_NAME_LEN];
	int quantity;
	double price_per_item;
	articleP next;
}article;

struct order;
typedef struct _order* orderP;
typedef struct _order {
	char item[MAX_NAME_LEN];
	int item_quantity;
	double item_price;
	orderP next;
}order;

typedef enum {
	ROLE_DELIVERER,
	ROLE_MANAGER
}Role;

struct employee;
typedef struct _employee* employeeP;
typedef struct _employee {
	char firstname[MAX_NAME_LEN];
	char lastname[MAX_NAME_LEN];
	char username[MAX_NAME_LEN];
	unsigned long password_hash;
	Role role;
	employeeP next;
}employee;


int openFileWarehouse(article* article_head);
void trimSpaces(char* str); //moran ovo imat za uspjesno citanje iz datoteke i narudzbe
int createList(article* article_head, char* name, int quantity, double price);
int printList(article* article_head); 
int homePage(article* article_head, employeeP employee_head);
int addToOrder(order* order_head, char* article_name, int article_quantity, double price);
articleP findArticle(article* article_head, char* name); 
int updateWarehouse(article* article_head, char* name, int quantity);
int createOrder(order* order_head, article* article_head);
int calculateTotal(order* order_head);
int printReceipt(order* order_head);
int freeOrderList(order* order_head);
int customerMenu(article* article_head);
unsigned long hashPassword(const char* password, const char* username);
int openFileEmployees(employeeP employee_head);
int createEmployeeList(employeeP employee_head, char* username, char* firstname, char* lastname, unsigned long password_hash, Role role);
employeeP findEmployee(employeeP employee_head, char* username);
int loginEmployee(employeeP employee_head, article* article_head, Role expected_role);
int saveEmployeesToFile(employeeP employee_head);
int managerMenu(employeeP employee_head, article* article_head);
int registerDeliverer(employeeP employee_head);
int deleteDeliverer(employeeP employee_head);
int viewAllDeliverers(employeeP employee_head);
int viewWarehouseStatus(article* article_head);
int delivererMenu(article* article_head);
int viewOrders();
int saveOrderToFile(order* order_head);


int main() {

	SetConsoleOutputCP(65001);  // UTF-8
	SetConsoleCP(65001);

	setlocale(LC_ALL, ".UTF-8"); //ovo mi treba za text_art


	article article_head = { .name = "", .quantity = 0, .price_per_item = 0.0 , .next = NULL };
	employee employee_head = { .firstname = "", .lastname="", .username="", .password_hash=0, .role = ROLE_DELIVERER, .next=NULL};

	//ucitavanje artikala iz skladista
	openFileWarehouse(&article_head);

	//ucitavanje zaposlenika
	openFileEmployees(&employee_head);

	homePage(&article_head, &employee_head);

	return 0;
}

int openFileWarehouse(article* head) {

	//otvaranje datoteke u modu za citanje i provjera
	FILE* fp = fopen("Warehouse.txt", "r");
	if (fp == NULL) {
		printf("Neuspjesno otvaranje datoteke!\n");
		return EXIT_FAILURE;
	}
	//citanje iz datoteke red po red
	char buffer[256];
	char name[MAX_NAME_LEN];
	int quantity;
	double price;
	while (fgets(buffer, 256, fp) != NULL) {
		sscanf(buffer, "%[^0-9] %d %lf", name, &quantity, &price); //%[^0-9] -> citaj sve OSIM brojeva
		trimSpaces(name);
		createList(head, name, quantity, price);
	}

	fclose(fp);
	return EXIT_SUCCESS;
}

void trimSpaces(char* str) {
	size_t len = strlen(str);
	// Ukloni razmake, tabove i newline s kraja stringa
	while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\n' || str[len - 1] == '\r')) {
		str[len - 1] = '\0';
		len--;
	}
}

//ova funkcija mi je za napravit vezanu listu artikala u skladistu
int createList(article* head, char* name, int quantity, double price) {
	//alokacija memorije i provjera
	articleP new = malloc(sizeof(article));
	if (new == NULL) {
		printf("Neuspjesna alokacija memorije!\n");
		return EXIT_FAILURE;
	}

	//postavljanje pokazivaca za kretanje kroz listu
	article* curr = head;

	//sadrzaj cvora article
	strcpy(new->name, name);
	new->quantity = quantity;
	new->price_per_item = price;
	new->next = NULL;

	//petlja kojom se krece do kraja liste
	while (curr->next != NULL) {

		curr = curr->next;
	}

	//postavljanje pokazivaca dodanog cvora
	new->next = curr->next;
	curr->next = new;

	return EXIT_SUCCESS;
}

int printList(article* head) {
	article* curr = head->next;
	while (curr != NULL) {
		printf("%-25s %12.2f\n", curr->name, curr->price_per_item);
		curr = curr->next;
	}

	return EXIT_SUCCESS;
}

int homePage( article* article_head, employeeP employee_head) {
	
	printf("\n");
	printf("  ╔══════════════════════════════════════════════════════╗\n");
	printf("  ║  ════════════════════════════════════════════════    ║\n");
	printf("  ║                                                      ║\n");
	printf("  ║            ┬ ┬┌─┐┬─┐┌─┐┬ ┬┌─┐┬ ┬┌─┐┌─┐               ║\n");
	printf("  ║            │││├─┤├┬┘├┤ ├─┤│ ││ │└─┐├┤                ║\n");
	printf("  ║            └┴┘┴ ┴┴└─└─┘┴ ┴└─┘└─┘└─┘└─┘               ║\n");
	printf("  ║                          &                           ║\n");
	printf("  ║                 ┬ ┬┌─┐┌┐ ┌─┐┬ ┬┌─┐┌─┐                ║\n");
	printf("  ║                 │││├┤ ├┴┐└─┐├─┤│ │├─┘                ║\n");
	printf("  ║                 └┴┘└─┘└─┘└─┘┴ ┴└─┘┴                  ║\n");
	printf("  ║                                                      ║\n");
	printf("  ║  ════════════════════════════════════════════════    ║\n");
	printf("  ╚══════════════════════════════════════════════════════╝\n");
	printf("\n");
	
	int choice=0;
	
	while (1) {
		printf("\n══════════════════DOBRO DOSLI!══════════════════\n");
		printf("Odaberi: \n");
		printf("1 - kupac\n");
		printf("2 - zaposlenik \n");
		printf("0 - kraj programa \n");
		printf("\n");
		printf("Odabir ");



		if (scanf("%d", &choice) != 1) {
			printf("Neispravan unos!\n");
			while (getchar() != '\n');
			continue;
		}
		while (getchar() != '\n');

		switch (choice) {
		case 1:

			printf("\nKUPAC\n");
			customerMenu(article_head);

			break;

		case 2: {
			printf("════════════════════════════════════════════════\n");
			printf("\n               ZAPOSLENIK                     \n");

			int roleChoice = 0;
			printf("Odaberi prijavu kao:\n");
			printf("1-poslovoda\n");
			printf("2-Ddostavljac\n");

			if (scanf("%d", &roleChoice) != 1) {
				printf("Neispravan unos!\n");
				while (getchar() != '\n');
				break;
			}
			while (getchar() != '\n'); // očisti buffer

			//podpetlja za odabir uloge
			switch (roleChoice) {

			case 1:

				printf("\n");
				printf("  ╔══════════════════════════════════════════════════════╗\n");
				printf("  ║              PRIJAVA KAO POSLOVODA                   ║\n");
				printf("  ╚══════════════════════════════════════════════════════╝\n");
				printf("\n");

				loginEmployee(employee_head, article_head, ROLE_MANAGER);
				break;

			case 2:
				printf("\n");
				printf("  ╔══════════════════════════════════════════════════════╗\n");
				printf("  ║              PRIJAVA KAO DOSTAVLJAC                  ║\n");
				printf("  ╚══════════════════════════════════════════════════════╝\n");
				printf("\n");

				loginEmployee(employee_head, article_head, ROLE_DELIVERER);
				break;

			default:
				printf("Neispravan odabir uloge!\n");
				break;
			}

			break;
		}

		case 0:
			printf("Izlaz iz programa\n");
			return EXIT_SUCCESS;

		default:
			printf("Neispravan odabir\n");
		}

	
	}

	return EXIT_SUCCESS;
}

int addToOrder(order* order_head, char* article_name, int article_quantity, double price) {

	//alokacija memorije i provjera
	orderP new_order = malloc(sizeof(order));
	if (new_order == NULL) {
		printf("Neuspjesna alokacija memorije!\n");

		return EXIT_FAILURE;
	}

	//pokazivac za prolazak po listi
	order* curr = order_head;

	//postavljanje pokazivaca
	strcpy(new_order->item, article_name); //artikl unesen sa tipkovnice povezi u listu
	new_order->item_quantity = article_quantity; //kolicinu unesenu sa tipkovnice povezi u listu
	new_order->item_price = price;
	new_order->next = NULL;

	while (curr->next != NULL) { //pomicanje pokazivaca curr sve dok se ne dode do kraja

		curr = curr->next;
	}

	new_order->next = curr->next;
	curr->next = new_order;

	return EXIT_SUCCESS;
}

/* Ova funkcija mi je potrebna za povezivanje artikala iz narudzbe
sa artiklima iz skladista */
articleP findArticle(article* article_head, char* name) {

	//postavljanje pokazivaca za lakse kretanje po listi
	article* curr = article_head->next;

	while (curr != NULL) {
		if (strcmp(curr->name, name) == 0) {
			return curr;
		}
		curr = curr->next;
	}

	return NULL;
}

//funkcija za azuriranje stanja skladista
int updateWarehouse(article* article_head, char* name, int quantity) {

	//pokazivac za kretanje kroz listu (krece se od prvog stvarnog elementa)
	article* curr = article_head->next;

	while (curr != NULL) {
		//AKO SE PODUDARA UNESENI ARTIKL SA ARTIKLON IZ SKLADISTA AZURIRA SE STANJE
		if (strcmp(curr->name, name) == 0) { 
			
			curr->quantity = curr->quantity - quantity;
			
			return EXIT_SUCCESS;
		}
		curr = curr->next;
	}

	return EXIT_FAILURE;
}

int createOrder(order* order_head, article* article_head) {

	//deklaracija varijabli koje ce nam trebat za napravit narudzbu
	char article_name[MAX_NAME_LEN];
	char input_line[256];
	int quantity;
	

	printf("\n");
	printf("  ╔══════════════════════════════════════════════════════╗\n");
	printf("  ║                 KREIRANJE NARUDZBE                   ║\n");
	printf("  ╚══════════════════════════════════════════════════════╝\n");
	printf("\n");

	printf("\nDostupni artikli:\n");
	printf("\n");

	printList(article_head);
	printf("\n");
	printf("Unesi artikl i kolicinu (ili KRAJ za zavrsetak narudzbe): \n");

	while (1) {
		

		if (fgets(input_line, sizeof(input_line), stdin) == NULL) {
			printf("Greska pri citanju!\n");
			continue;
		}
		
		if (strncmp(input_line, "KRAJ", 4) == 0) {
			break;
		}

		if (sscanf(input_line, " %[^0-9] %d", article_name, &quantity) != 2) {
			printf("Neispravan format! Unesi: 'naziv artikla količina' (npr: 'sir gauda 5')\n");
			continue;
		}

		trimSpaces(article_name);


		articleP found = findArticle(article_head, article_name);

		if (found == NULL) {
			printf("Artikl '%s' ne postoji u skladistu!\n", article_name);
			continue;
		}

		if (quantity <= 0) {
			printf("Kolicina mora biti veca od 0!\n");
			continue;
		}

		if(found->quantity < quantity) {
			printf("Nema dovoljno artikala na skladistu! Dostupno: %d\n", found->quantity);
			continue;
		}

		addToOrder(order_head, article_name, quantity, found->price_per_item);
		updateWarehouse(article_head, article_name, quantity);
			
	}

	return EXIT_SUCCESS;

}

int calculateTotal(order* order_head) {

	order* curr = order_head->next;
	double total = 0.0;

	while (curr != NULL) {
		total += (curr->item_quantity * curr->item_price);
		curr = curr->next;
	}

	printf("\n");
	printf("  ═════════════════════════════════════════════════\n");
	printf("         UKUPNA CIJENA : %.2lf euro\n              \n", total);
	printf("  ═════════════════════════════════════════════════\n");
	printf("\n");

	return EXIT_SUCCESS;
}
int printReceipt(order* order_head) {

	order* curr = order_head->next;

	printf("\n%-30s %-10s %-10s %-10s\n", "ARTIKL", "KOLICINA", "CIJENA", "UKUPNO");
	printf("--------------------------------------------------------------------\n");

	while (curr != NULL) {
		double subtotal = curr->item_quantity * curr->item_price;
		printf("%-30s %-10d %.2lf EURO   %.2lf EURO \n",
			curr->item, curr->item_quantity, curr->item_price, subtotal);
		curr = curr->next;
	}

	return EXIT_SUCCESS;
}

int freeOrderList(order* order_head) {

	order* curr = order_head->next;
	order* temp;

	while (curr != NULL) {
		temp = curr;
		curr = curr->next;
		free(temp);
	}

	order_head->next = NULL;

	return EXIT_SUCCESS;

}

int customerMenu(article* article_head) {

	order order_head = { .item = "", .item_quantity = 0, .item_price=0.0, .next = NULL };

	int result = createOrder(&order_head, article_head);

	if (result == EXIT_SUCCESS) {
		printf("\n");
		printf("  ╔══════════════════════════════════════════════════════╗\n");
		printf("  ║                 DOVRSENA NARUDZBA                    ║\n");
		printf("  ╚══════════════════════════════════════════════════════╝\n");
		printf("\n");
		printReceipt(&order_head);
		calculateTotal(&order_head);

		//funkcija za spremanje narudzbe u datoteku (imitaciju baze)
		saveOrderToFile(&order_head);
	}

	freeOrderList(&order_head);

	return EXIT_SUCCESS;
}

unsigned long hashPassword(const char* password, const char* username) {
	unsigned long hash = 2166136261u;

	//Hash username kao "salt"
	const char* salt = username;
	while (*salt) {
		hash ^= (unsigned char)(*salt++);
		hash *= 16777619;
	}

	//Hash password
	while (*password) {
		hash ^= (unsigned char)(*password++);
		hash *= 16777619;
	}

	return hash;
}

int openFileEmployees(employeeP employee_head) {

	FILE* fp = fopen("employees.txt", "r");
	if (fp == NULL) {
		printf("Neuspjesno otvaranje datoteke!\n");
		return EXIT_FAILURE;
	}

	char buffer[256];
	char username[MAX_NAME_LEN];
	char firstname[MAX_NAME_LEN];
	char lastname[MAX_NAME_LEN];
	unsigned long password_hash;
	int role_int;

	while (fgets(buffer, 256, fp) != NULL) {
		if (sscanf(buffer, "%s %s %s %lu %d", username, firstname, lastname, &password_hash, &role_int) == 5) {
			Role role;
			if (role_int == 0) {
				role = ROLE_DELIVERER;
			}
			else {
				role = ROLE_MANAGER;
			}
			//FUNKCIJA ZA KREIRANJE LISTE ZAPOSLENIKA
			createEmployeeList(employee_head, username, firstname, lastname, password_hash, role);
		}
	}

	fclose(fp);

	return EXIT_SUCCESS;
}

int createEmployeeList(employeeP employee_head, char* username, char* firstname, char* lastname, unsigned long password_hash, Role role) {

	//alokacija meorije i provjera
	employeeP new_employee = malloc(sizeof(employee));
	if (new_employee == NULL) {
		printf("Neuspjesna alokacija memorije!\n");
		return EXIT_FAILURE;
	}

	//postavljanje pokazivaca za kretanje po listi
	employeeP curr = employee_head;

	//sadrzaj cvora
	strcpy(new_employee->username, username);
	strcpy(new_employee->firstname, firstname);
	strcpy(new_employee->lastname, lastname);
	new_employee->password_hash = password_hash;
	new_employee->role = role;
	new_employee->next = NULL;

	//prolazak kroz petlju dok se ne dode do zadnjeg clana
	while (curr->next != NULL) {
		curr = curr->next;
	}

	//povezivanje novo dodanog cvora u listu
	new_employee->next = curr->next;
	curr->next = new_employee;

	return EXIT_SUCCESS;
}

employeeP findEmployee(employeeP employee_head, char* username) {

	//postavljanje pokazivaca za lakse kretanje po listi (prvi stvarni el)
	employeeP curr = employee_head->next;

	while (curr != NULL) {
		if (strcmp(curr->username, username) == 0) {
			return curr;
		}
		curr = curr->next;
	}

	return NULL;
}

int loginEmployee(employeeP employee_head, article* article_head, Role expected_role) {

	char username[MAX_NAME_LEN];
	char password[MAX_NAME_LEN];

	printf("Username: ");
	if (scanf("%31s", username) != 1) {
		printf("Greska pri citanju!\n");
		while (getchar() != '\n');
		return EXIT_FAILURE;
	}

	printf("Password: ");
	if (scanf("%31s", password) != 1) {
		printf("Greska pri citanju!\n");
		while (getchar() != '\n');
		return EXIT_FAILURE;
	}
	while (getchar() != '\n');

	//hashiranje unesene lozinke
	unsigned long entered_hash = hashPassword(password, username);

	//Pronadi zaposlenika, tj usporedi uneseni username sa onim u datoteci
	employeeP employee = findEmployee(employee_head, username);

	if (employee == NULL) {
		printf("Korisnik ne postoji!\n");
		return EXIT_FAILURE;
	}

	//Provjeri lozinku, tj usporedi unesenu sa onom u datoteci
	if (employee->password_hash != entered_hash) {
		printf("Netocna lozinka!\n");
		return EXIT_FAILURE;
	}
	
	//Provjeri ulogu
	if (employee->role != expected_role) {
		printf("Nemate pristup ovoj ulozi!\n");
		return EXIT_FAILURE;
	}

	//ako su sve provjere prosle, dozvoli prijavu
	printf("\nDobrodosli, %s %s!\n", employee->firstname, employee->lastname);

	if (expected_role == ROLE_MANAGER) {
		managerMenu(employee_head, article_head);
	}
	else {
		delivererMenu(article_head);
	}

	return EXIT_SUCCESS;
}

//spremanje zaposlenika u datoteku
int saveEmployeesToFile(employeeP employee_head) {
	//otvaranje datoteke u modu za pisanje i provjera je li otvorena ispravno
	FILE* fp = fopen("employees.txt", "w"); 
	if (fp == NULL) {
		printf("Neuspjesno otvaranje datoteke!\n");
		return EXIT_FAILURE;
	}

	//postavljanje pokazivaca za lakse kretanje kroz listu (prvi stvarni el)
	employeeP curr = employee_head->next;

	while (curr != NULL) {

		fprintf(fp, "%s %s %s %lu %d\n", curr->username, curr->firstname, curr->lastname, curr->password_hash, curr->role);
		
		curr = curr->next;
	}

	fclose(fp);

	return EXIT_SUCCESS;
 }

int managerMenu(employeeP employee_head, article* article_head) {

	int choice = 0;
	
	while (1) {
		printf("\n");
		printf("  ╔══════════════════════════════════════════════════════╗\n");
		printf("  ║                  IZBORNIK POSLOVODE                  ║\n");
		printf("  ╚══════════════════════════════════════════════════════╝\n");
		printf("\n");
		printf("1 - Registracija novog dostavljaca\n");
		printf("2 - Brisanje dostavljaca\n");
		printf("3 - Pregled dostavljaca\n");
		printf("4 - Stanje skladista\n");
		printf("0 - Odjava\n");
		printf("Odabir: ");

		if (scanf("%d", &choice) != 1) {
			printf("Neispravan unos!\n");
			while (getchar() != '\n');
			continue;
		}
		while (getchar() != '\n');

		switch (choice) {
		case 1:
			registerDeliverer(employee_head);
			break;
		case 2:
			deleteDeliverer(employee_head);
			break;
		case 3:
			viewAllDeliverers(employee_head);
			break;
		case 4:
			viewWarehouseStatus(article_head);
			break;
		case 0:
			printf("Odjava...\n");
			return EXIT_SUCCESS;
		default:
			printf("Neispravan odabir!\n");
		}
	}

	return EXIT_SUCCESS;
}

//funkcija za registraciju dostavljaca
int registerDeliverer(employeeP employee_head) {

	char username[MAX_NAME_LEN];
	char firstname[MAX_NAME_LEN];
	char lastname[MAX_NAME_LEN];
	char password[MAX_NAME_LEN];

	printf("  ════════════════════════════════════════════════ \n");
	printf("           REGISTRACIJA NOVOG DOSTAVLJACA          \n");
	printf("  ════════════════════════════════════════════════ \n");

	printf("Username: ");
	if (scanf("%31s", username) != 1) {
		printf("Greska pri citanju!\n");
		while (getchar() != '\n');
		return EXIT_FAILURE;
	}

	//provjera postoji li vec korisnik
	if (findEmployee(employee_head, username) != NULL) {
		printf("Korisnik s tim username-om vec postoji!\n");
		while (getchar() != '\n');
		return EXIT_FAILURE;
	}

	printf("Ime: ");
	if (scanf("%31s", firstname) != 1) {
		printf("Greska pri citanju!\n");
		while (getchar() != '\n');
		return EXIT_FAILURE;
	}

	printf("Prezime: ");
	if (scanf("%31s", lastname) != 1) {
		printf("Greska pri citanju!\n");
		while (getchar() != '\n');
		return EXIT_FAILURE;
	}

	printf("Lozinka: ");
	if (scanf("%31s", password) != 1) {
		printf("Greska pri citanju!\n");
		while (getchar() != '\n');
		return EXIT_FAILURE;
	}
	while (getchar() != '\n');

	//HASHIRANJE LOZINKE
	unsigned long password_hash = hashPassword(password, username);

	//dodaj registriranog korisnika u listu
	createEmployeeList(employee_head, username, firstname, lastname, password_hash, ROLE_DELIVERER);

	//spremi u datoteku (imitacija baze)
	saveEmployeesToFile(employee_head);

	printf("Dostavljac %s %s uspjesno registriran!\n", firstname, lastname);

	return EXIT_SUCCESS;
}

//funkcija za brisanje dostavljaca
int deleteDeliverer(employeeP employee_head) {

	char username[MAX_NAME_LEN];

	printf("  ════════════════════════════════════════════════ \n");
	printf("                 BRISANJE DOSTAVLJACA              \n");
	printf("  ════════════════════════════════════════════════ \n");

	//poziv funkcije za ispis svih dostavljaca
	viewAllDeliverers(employee_head);

	printf("\nUnesi username dostavljaca za brisanje: ");
	if (scanf("%31s", username) != 1) {
		printf("Greska pri citanju!\n");
		while (getchar() != '\n');
		return EXIT_FAILURE;
	}
	while (getchar() != '\n');

	//postavljanje pokazivaca za lakse kretanje kroz listu
	employeeP curr = employee_head;
	employeeP prev = NULL;

	//petlja za prolazak kroz listu
	while (curr->next != NULL) {

		prev = curr;
		curr = curr->next;

		//ako se uneseno ime poklapa sa imenom iz liste - ukloni iz liste
		if (strcmp(curr->username, username) == 0) {

			prev->next = curr->next;
			free(curr);

			//spremi u datoteku
			saveEmployeesToFile(employee_head);

			printf("Dostavljac '%s' uspjesno obrisan!\n", username);

			return EXIT_SUCCESS;
		}
	}

	printf("Dostavljac s usernamom '%s' ne postoji!\n", username);

	return EXIT_FAILURE;
}

int viewAllDeliverers(employeeP employee_head) {

	//postavljanje klasik pomocnog pokazivaca
	employeeP curr = employee_head->next;

	int count = 0;

	printf("                   POPIS DOSTAVLJACA              \n");
	printf("  ══════════════════════════════════════════════════ \n");
	printf("%-20s %-20s %-20s\n", "USERNAME", "IME", "PREZIME");
	printf("  ══════════════════════════════════════════════════ \n");

	while (curr != NULL) {
		if (curr->role == ROLE_DELIVERER) {
			printf("%-20s %-20s %-20s\n", curr->username, curr->firstname, curr->lastname);
			count++;
		}
		curr = curr->next;
	}

	if (count == 0) {
		printf("Nema registriranih dostavljaca.\n");
	}
	else {
		printf("\nUkupno dostavljaca: %d\n", count);
	}

	return EXIT_SUCCESS;
}

int viewWarehouseStatus(article* article_head) {

	article* curr = article_head->next;

	printf("                    STANJE SKLADISTA                 \n");
	printf("  ══════════════════════════════════════════════════ \n");
	printf("%-30s %-15s\n", "ARIKL", "KOLICINA");

	while (curr != NULL) {
		printf("%-30s %-15d\n", curr->name, curr->quantity);
		curr = curr->next;
	}
	printf("\n");

	return EXIT_SUCCESS;
}


int delivererMenu(article* article_head) {

	int choice = 0;

	while (1) {
		printf("\n");
		printf("  ╔══════════════════════════════════════════════════════╗\n");
		printf("  ║                  IZBORNIK DOSTAVLJACA                ║\n");
		printf("  ╚══════════════════════════════════════════════════════╝\n");
		printf("\n");
		printf("1 - Pregled narudzbi\n");
		printf("0 - Odjava\n");
		printf("Odabir: ");

		if (scanf("%d", &choice) != 1) {
			printf("Neispravan unos!\n");
			while (getchar() != '\n');
			continue;
		}
		while (getchar() != '\n');

		switch (choice) {
		case 1:
			viewOrders();
			break;
		case 0:
			printf("Odjava...\n");
			return EXIT_SUCCESS;
		default:
			printf("Neispravan odabir!\n");
		}
	}

	return EXIT_SUCCESS;
}

int viewOrders() {

	FILE* fp = fopen("orders.txt", "r");
	if (fp == NULL) {
		printf("Nema narudzbi ili greska pri ovaranju datoteke!\n");
		return EXIT_FAILURE;
	}

	char buffer[256];

	printf("                    POPIS NARUDZBI                    \n");
	printf("══════════════════════════════════════════════════════\n");

	while (fgets(buffer, 256, fp) != NULL) {
		printf("%s", buffer);
	}

	fclose(fp);
	return EXIT_SUCCESS;
}

int saveOrderToFile(order* order_head) {

	FILE* fp = fopen("orders.txt", "a"); //append mod
	if (fp == NULL) {
		printf("Neuspjesno otvaranje datoteke orders.txt!\n");
		return EXIT_FAILURE;
	}

	order* curr = order_head->next;
	double total = 0.0;

	//upisivanje u datoteku
	while (curr != NULL) {
		double subtotal = curr->item_quantity * curr->item_price;
		fprintf(fp, "%s x%d - %.2lf EURO\n", curr->item, curr->item_quantity, subtotal);
		total += subtotal;
		curr = curr->next;
	}

	fprintf(fp, "        UKUPNO: %.2lf EURO         \n", total);

	fclose(fp);
	return EXIT_SUCCESS;
}