/* STATUS: Changed Lost and Found to Findora, Added Found Location for Found Item
 
---Group Members---
- Muhammad Saim
- Syed Zain Raza Abbas
- Shan -E- Ali

---Project Name---
- Findora

*/


#include <iostream>
#include <string>
#include <conio.h>
#include <unistd.h>
using namespace std;


enum MenuOptions {
    LOST_ITEM          = '1',
    FOUND_ITEM         = '2',
    SEARCH_ITEM        = '3',
    DISPLAY_DETAILS    = '4',
    EXIT               = '5',
};



void directing(); 
void findora();
void findora_animate(); 
void display_inputs(struct Details *p, enum MenuOptions option);
void display_details(struct Details *p, enum MenuOptions option);
bool is_input_blank(struct Details *p, enum MenuOptions option);

unsigned int DELAY = 60000;   /*41900*/
unsigned int DELAY_2 = 29900;   /*41900*/
/* bool is_lost_active = false; */
/* bool is_found_active = false; */

struct Details {
    string Name;
    string ID;
    string Category;
    string Item_Name;
    string Brand_Name;
    string Item_Color;
    string Unique_ID;
    string Last_Seen_Location;
    string Found_Location;
    string Date_Time;
    string Contact_Info;
};



int main() {

    struct Details LI_person;
    struct Details FI_person;

    int menu_option;
    int display_details_option = 0;
    bool is_first_launch = false;

    system("cls");
    cout << "\n\n";
    cout << "  Lost and Found Smart Campus" << endl;
    cout << "--------------------------------------------";
    cout << "\n\n\n";

    cout << "   Enter Name \n\n > ";
    getline(cin, LI_person.Name);


    if (LI_person.Name.empty()) {
        cout << endl << " Error: No input found" << endl;
        return 0;
    }

    cout << "\n\n";

    cout << "   Enter ID \n\n > ";
    getline(cin, LI_person.ID);

    while (LI_person.ID.length() < 8) {
        cout << endl << " Error: Invalid ID.Please try again" << endl << endl;
        cout << " Enter ID: ";
        getline(cin, LI_person.ID);
    }//end of while()

    FI_person.Name = LI_person.Name;
    FI_person.ID   = LI_person.ID;




    directing();
start:

    do { //Main loop

        do { //Menu loop

            system("cls");

            if (is_first_launch) {
                findora();
            }

            else{
                findora_animate();
            }

            cout << "\n\n\n\n";
            cout << "\t\t\t\t\t\t    [1] Report Lost Item" << endl << endl;
            cout << "\t\t\t\t\t\t    [2] Report Found Item" << endl << endl;
            cout << "\t\t\t\t\t\t    [3] Search Items" << endl << endl;
            cout << "\t\t\t\t\t\t    [4] Display Details" << endl << endl;
            cout << "\t\t\t\t\t\t    [5] Exit" << endl << endl;

            cout << endl << "\t\t\t\t\t\t    Enter option : ";
            menu_option = getche();

            if (menu_option == EXIT) {

                while (menu_option != 'n' && menu_option != 'N' && menu_option != 'y' && menu_option != 'Y') {
                cout << "\n\n\t\t\t\t\t\t    Exit? (y/n) : ";
                menu_option = getche();

                if (menu_option == 'y' || menu_option == 'Y') {
                    cout << endl << endl << endl << " Exited successfully...";
                    return 0;
                }

                if (menu_option == 'n' || menu_option == 'N') {
                    is_first_launch = true;
                    goto start;
                }

                }// end of while()
            }//end of outer if()

            if (menu_option == DISPLAY_DETAILS) {

                cout << endl << endl << "\t\t\t\t\t\t    [1] Lost Item Details" << endl;
                cout << "\t\t\t\t\t\t    [2] Found Item Details" << endl << endl;

                do {
                    cout << "\t\t\t\t\t\t    Option : ";
                    display_details_option = getche();

                    if (display_details_option == LOST_ITEM || display_details_option == FOUND_ITEM) {
                        break;
                    }

                    if (display_details_option != LOST_ITEM && display_details_option != FOUND_ITEM) {
                        cout << endl << endl << " \t\t\t\t\t\t    Error: Invalid Option" << endl;
                    }
                }while(display_details_option != LOST_ITEM && display_details_option != FOUND_ITEM);
            }// end of outer if()

            if (menu_option == LOST_ITEM || menu_option == FOUND_ITEM || menu_option == SEARCH_ITEM || menu_option == DISPLAY_DETAILS) {
                break;
            }

        }while (menu_option != LOST_ITEM && menu_option != FOUND_ITEM && menu_option != SEARCH_ITEM && menu_option != DISPLAY_DETAILS && menu_option != EXIT); // end of Menu Loop

        system("cls");

        switch (menu_option) {

            case LOST_ITEM:
                cout << endl << endl << "  Report Lost Item" << endl;
                cout << "----------------------------------" << endl << endl << endl;
                display_inputs(&LI_person, LOST_ITEM);

                if (!is_input_blank(&LI_person, LOST_ITEM)) {
                    cout << endl << "  All details are provided.";
                }
                break;


            case FOUND_ITEM:
                cout << endl << endl << "  Report Found Item" << endl;
                cout << "----------------------------------" << endl << endl << endl;
                display_inputs(&FI_person, FOUND_ITEM);

                if (!is_input_blank(&FI_person, FOUND_ITEM)) {
                    cout << endl <<  "  All details are provided.";
                }
                break;


            case SEARCH_ITEM:
                cout << endl << endl << "  Search Item" << endl;
                cout << "----------------------------------";
                break;


            case DISPLAY_DETAILS:
                if (display_details_option == LOST_ITEM) {

                    cout << endl << endl << "  Lost Item Details" << endl;
                    cout << "----------------------------------" << endl << endl << endl;
                    display_details(&LI_person, LOST_ITEM);
                }

                if (display_details_option == FOUND_ITEM) {

                    cout << endl << endl << "  Found Item Details" << endl;
                    cout << "----------------------------------" << endl << endl << endl;
                    display_details(&FI_person, FOUND_ITEM);
                }
                break;



        }//end of switch()

        cout << endl << endl << "  Press any key... ";
        (void)getch();
        is_first_launch = true;


    }while (1); //end of Main Loop

    return 0;

}//end of main()


void findora_animate() {

    cout << endl << endl;

    cout << "\t\t\t\t$$$$$$$$\\ $$\\                 $$\\                               \n";
    usleep(DELAY);
    cout << "\t\t\t\t$$  _____|\\__|                $$ |                              \n";
    usleep(DELAY);
    cout << "\t\t\t\t$$ |      $$\\ $$$$$$$\\   $$$$$$$ | $$$$$$\\   $$$$$$\\  $$$$$$\\   \n";
    usleep(DELAY);
    cout << "\t\t\t\t$$$$$\\    $$ |$$  __$$\\ $$  __$$ |$$  __$$\\ $$  __$$\\ \\____$$\\  \n";
    usleep(DELAY);
    cout << "\t\t\t\t$$  __|   $$ |$$ |  $$ |$$ /  $$ |$$ /  $$ |$$ |  \\__|$$$$$$$ | \n";
    usleep(DELAY);
    cout << "\t\t\t\t$$ |      $$ |$$ |  $$ |$$ |  $$ |$$ |  $$ |$$ |     $$  __$$ | \n";
    usleep(DELAY);
    cout << "\t\t\t\t$$ |      $$ |$$ |  $$ |\\$$$$$$$ |\\$$$$$$  |$$ |     \\$$$$$$$ | \n";
    usleep(DELAY);
    cout << "\t\t\t\t\\__|      \\__|\\__|  \\__| \\_______| \\______/ \\__|      \\_______| \n";
    usleep(DELAY);
} 

void findora() {

    cout << endl << endl;

    cout << "\t\t\t\t$$$$$$$$\\ $$\\                 $$\\                               \n";
    cout << "\t\t\t\t$$  _____|\\__|                $$ |                              \n";
    cout << "\t\t\t\t$$ |      $$\\ $$$$$$$\\   $$$$$$$ | $$$$$$\\   $$$$$$\\  $$$$$$\\   \n";
    cout << "\t\t\t\t$$$$$\\    $$ |$$  __$$\\ $$  __$$ |$$  __$$\\ $$  __$$\\ \\____$$\\  \n";
    cout << "\t\t\t\t$$  __|   $$ |$$ |  $$ |$$ /  $$ |$$ /  $$ |$$ |  \\__|$$$$$$$ | \n";
    cout << "\t\t\t\t$$ |      $$ |$$ |  $$ |$$ |  $$ |$$ |  $$ |$$ |     $$  __$$ | \n";
    cout << "\t\t\t\t$$ |      $$ |$$ |  $$ |\\$$$$$$$ |\\$$$$$$  |$$ |     \\$$$$$$$ | \n";
    cout << "\t\t\t\t\\__|      \\__|\\__|  \\__| \\_______| \\______/ \\__|      \\_______| \n";
}


void directing() {

    cout << "\n\n  Directing";
    usleep(95000); 
    cout << ".";
    usleep(95000); 
    cout << ".";
    usleep(95000); 
    cout << ".";
    usleep(99999); 
    cout << "\n";
} 

bool is_input_blank(struct Details *p, enum MenuOptions option) {

    if(p->Category.empty()   || p->Item_Name.empty()  || p->Brand_Name.empty() ||
       p->Item_Color.empty() || p->Unique_ID.empty()  || 
       p->Date_Time.empty()  || p->Contact_Info.empty()) {

        return true;
    }


    if (option == LOST_ITEM && p->Last_Seen_Location.empty()) {
        return true;
    }

    if (option == FOUND_ITEM && p->Found_Location.empty()) {
        return true;
    }

    return false;
}

void display_inputs(struct Details *p, enum MenuOptions option) {

    if (p->Category.empty()) {
        cout << "  For Example: [Electronics | Documents | Apparel | Valuables | Other]" << endl << endl; 
        cout << "  Enter Item Category: ";
        getline(cin, p->Category);
    }

    if (p->Item_Name.empty()) {
        cout << endl << "  Enter Item Name: ";
        getline(cin, p->Item_Name);
    }

    if (p->Brand_Name.empty()) {
        cout << endl << "  Enter Brand Name: ";
        getline(cin, p->Brand_Name);
    }

    if(p->Item_Color.empty()) {
        cout << endl << "  Enter Color: ";
        getline(cin, p->Item_Color);
    }

    if (p->Unique_ID.empty()) {
        cout << endl << "  For Example: [Serial # | Stickers | dents | Custom marks | Other]" << endl; 
        cout << endl << "  Enter Unique Identifiers: ";
        getline(cin, p->Unique_ID);
    }

    if (option == LOST_ITEM) {

    if (p->Last_Seen_Location.empty()) {
        cout << endl << "  Enter Last Seen Location: ";
        getline(cin, p->Last_Seen_Location);
    }

    }

    else {

    if (p->Found_Location.empty()) {
        cout << endl << "  Enter Found Location: ";
        getline(cin, p->Found_Location);
    }

    }


    if (p->Date_Time.empty()) {
        cout << endl << "  Enter Date and Time (DD-MM-YYYY HH:MM AM/PM): ";
        getline(cin, p->Date_Time);
    }

    if (p->Contact_Info.empty()) {
        cout << endl << "  Enter Contact Info \n(For Example: [Phone Number | Gmail | Other]): ";
        getline(cin, p->Contact_Info);
    }
}

void display_details(struct Details *p, enum MenuOptions option) {

    usleep(DELAY_2);
    cout << "   Name                : " << p->Name << endl;
    usleep(DELAY_2);
    cout << "   ID                  : " << p->ID << endl;
    usleep(DELAY_2);
    cout << "   Category            : " << p->Category << endl;
    usleep(DELAY_2);
    cout << "   Item name           : " << p->Item_Name << endl;
    usleep(DELAY_2);
    cout << "   Brand name          : " << p->Brand_Name << endl;
    usleep(DELAY_2);
    cout << "   Item color          : " << p->Item_Color << endl;
    usleep(DELAY_2);
    cout << "   Unique ID           : " << p->Unique_ID << endl;
    usleep(DELAY_2);

    if (option == LOST_ITEM) {
    cout << "   Last seen location  : " << p->Last_Seen_Location << endl;
    usleep(DELAY_2);
    }

    else {
    cout << "   Found Location      : " << p->Found_Location << endl;
    usleep(DELAY_2);
    }

    cout << "   Date time           : " << p->Date_Time << endl;
    usleep(DELAY_2);
    cout << "   Contact info        : " << p->Contact_Info << endl;
    usleep(DELAY_2);
}
