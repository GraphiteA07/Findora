
-- Project Name --
- Findora

-- Group Members --
- Muhammad Saim
- Syed Zain Raza Abbas
- Shan -E- Ali

#include <iostream>
#include <string>
#include <conio.h>
#include <unistd.h>
#include <curl/curl.h>
#include "json.hpp"
#include <cctype> //used for letter mtching in Searh Items.
#include <algorithm> //used for minimum difference in Searh Items.
using namespace std;


void directing(); 
void findora();
void findora_animate(); 
void display_LI_inputs(struct Details *LI_person);
void display_FI_inputs(struct Details *FI_person);
bool is_LI_input_blank(struct Details *LI_person);
bool is_FI_input_blank(struct Details *FI_person);
bool save_lost_item(struct Details *p);
bool save_found_item(struct Details *p);
bool save_match_request(string lost_id,string found_id, double percentage, string student_id);

unsigned int DELAY = 60000;   /*41900*/


struct Details {
    string Name;
    string ID;
    string Category;
    string Item_Name;
    string Brand_Name;
    string Item_Color;
    string Unique_ID;
    string Last_Seen_Location;
    string Date_Time;
    string Contact_Info;
    
};

enum {
    LOST_ITEM          = '1',
    FOUND_ITEM         = '2',
    SEARCH_ITEM        = '3',
    DISPLAY_DETAILS    = '4',
    MY_HISTORY         = '5',
    EXIT               = '6',
};

    struct Details LI_person;
    struct Details FI_person;
    
    nlohmann::json lost_items_data;
    nlohmann::json found_items_data;
     
    string json_escape(string s) {
    string result = "";

    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '"')
            result += "\\\"";
        else if (s[i] == '\\')
            result += "\\\\";
        else if (s[i] == '\n')
            result += "\\n";
        else if (s[i] == '\r')
            result += "\\r";
        else
            result += s[i];
    }

    return result;
}

size_t WriteCallback(void *contents, size_t size,
                     size_t nmemb, string *output) {

    size_t total_size = size * nmemb;

    output->append((char*)contents, total_size);

    return total_size;
}
string get_json_field(nlohmann::json item, string field) {

    if (item.contains(field) && !item[field].is_null()) {

        if (item[field].is_string()) {
            return item[field].get<string>();
        }

        else {
            return item[field].dump();
        }
    }

    return "";
}
int edit_distance(string a, string b) {

    int m = a.length();
    int n = b.length();

    int dp[101][101];

    for (int i = 0; i <= m; i++)
        dp[i][0] = i;

    for (int j = 0; j <= n; j++)
        dp[0][j] = j;

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {

            int cost = (tolower(a[i - 1]) ==
                        tolower(b[j - 1])) ? 0 : 1;

            dp[i][j] = min(
                dp[i - 1][j] + 1,
                min(
                    dp[i][j - 1] + 1,
                    dp[i - 1][j - 1] + cost
                )
            );
        }
    }

    return dp[m][n];
}

// Calculate spelling similarity percentage
double similarity_percentage(string a, string b) {

    if (a.empty() || b.empty()) {
        return 0;
    }

    int distance = edit_distance(a, b);

    int max_length = max(a.length(), b.length());

    double similarity =
        (1.0 - (double)distance / max_length) * 100;

    return similarity;
}

bool is_synonym(string a, string b) {

    string groups[15][5] = {

        {"Mobile", "Phone", "Smartphone", "Cellphone", ""},
        {"Laptop", "Notebook", "Computer", "", ""},
        {"Bag", "Backpack", "Schoolbag", "", ""},
        {"Bottle", "Water Bottle", "Flask", "", ""},
        {"Wallet", "Purse", "", "", ""},
        {"Watch", "Wristwatch", "", "", ""},
        {"Earphones", "Headphones", "Headset", "", ""},
        {"Charger", "Charging Adapter", "", "", ""},
        {"Shoes", "Sneakers", "Footwear", "", ""},
        {"Keys", "Keychain", "", "", ""},
        {"Pen", "Ballpoint", "Ball Pen", "", ""},
        {"Copy", "Exercise Book", "Notebook", "", ""},
        {"Glasses", "Spectacles", "Eyeglasses", "", ""},
        {"Umbrella", "Rain Umbrella", "", "", ""},
        {"Tablet", "iPad", "", "", ""}
    };

    for (int i = 0; i < 15; i++) {

        bool first_found = false;
        bool second_found = false;

        for (int j = 0; j < 5; j++) {

            string word = groups[i][j];

            if (word == "")
                continue;

            if (similarity_percentage(a, word) >= 85)
                first_found = true;

            if (similarity_percentage(b, word) >= 85)
                second_found = true;
        }

        if (first_found && second_found)
            return true;
    }

    return false;
}

double calculate_match_percentage(
    nlohmann::json lost,
    nlohmann::json found) {

    double percentage = 0;

    // Category - 15%
    if (similarity_percentage(
        get_json_field(lost, "category"),
        get_json_field(found, "category")) >= 85) {
        percentage += 15;
    }

    // Item Name - 20%
string lost_name = get_json_field(lost, "item_name");
string found_name = get_json_field(found, "item_name");

if (similarity_percentage(lost_name, found_name) >= 85 ||
    is_synonym(lost_name, found_name)) {

    percentage += 20;
}

    // Brand Name - 15%
    if (similarity_percentage(
        get_json_field(lost, "brand_name"),
        get_json_field(found, "brand_name")) >= 85) {
        percentage += 15;
    }

    // Item Color - 10%
    if (similarity_percentage(
        get_json_field(lost, "item_color"),
        get_json_field(found, "item_color")) >= 85) {
        percentage += 10;
    }

    // Unique ID - 20% (Exact match only)
    if (similarity_percentage(
	get_json_field(lost, "unique_id"),
	get_json_field(found, "unique_id")) >=
	85) {
        percentage += 20;
    }

    // Location - 10%
    if (similarity_percentage(
        get_json_field(lost, "last_seen_location"),
        get_json_field(found, "last_seen_location")) >= 85) {
        percentage += 10;
    }

    // Date and Time - 10% (Exact match only)
    if (get_json_field(lost, "date_time") != "" &&
        get_json_field(lost, "date_time") ==
        get_json_field(found, "date_time")) {
        percentage += 10;
    }

    return percentage;
}

void get_lost_items() {

    CURL *curl;
    CURLcode res;

    string response_body = "";

    curl = curl_easy_init();

    if (curl) {

        struct curl_slist *headers = NULL;

        string API_KEY = "sb_publishable_UimWqc3sbw01H__BPeiFbg_MlMyntMg";
        string key_header = "apikey: " + API_KEY;
        headers = curl_slist_append(headers,key_header.c_str());
        
        curl_easy_setopt(curl, CURLOPT_URL,
        "https://ybykqrntfmruxyouzily.supabase.co/rest/v1/lost_items?select=*");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {

            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

            cout << "\nHTTP Code: " << http_code << endl;
            cout << "\nLost Items Data:\n" << response_body << endl;
            
            try {

    nlohmann::json items =
        nlohmann::json::parse(response_body);
        lost_items_data = items;

    cout << "\nTotal Lost Items: " << items.size() << endl;

    for (int i = 0; i < items.size(); i++) {

        cout << "\nItem " << i + 1 << endl;

        cout << "Name: " << items[i]["name"] << endl;

        cout << "Item Name: "
             << items[i]["item_name"] << endl;
    }

}
catch (nlohmann::json::exception &e) {

    cout << "\nJSON Error: " << e.what() << endl;

}

        }
        else {
            cout << "GET Request Failed: "
                 << curl_easy_strerror(res) << endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}
void get_found_items() {

    CURL *curl;
    CURLcode res;

    string response_body = "";

    curl = curl_easy_init();

    if (curl) {

        struct curl_slist *headers = NULL;

        string API_KEY = "sb_publishable_UimWqc3sbw01H__BPeiFbg_MlMyntMg";
        string key_header = "apikey: " + API_KEY;

        headers = curl_slist_append(headers, key_header.c_str());

        curl_easy_setopt(curl, CURLOPT_URL,
        "https://ybykqrntfmruxyouzily.supabase.co/rest/v1/found_items?select=*");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {

            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

            cout << "\nHTTP Code: " << http_code << endl;
            cout << "\nFound Items Data:\n" << response_body << endl;
            
            try {

    nlohmann::json items =
        nlohmann::json::parse(response_body);
        found_items_data = items;

    cout << "\nTotal Found Items: " << items.size() << endl;

    for (int i = 0; i < items.size(); i++) {

        cout << "\nItem " << i + 1 << endl;

        cout << "Name: " << items[i]["name"] << endl;

        cout << "Item Name: "
             << items[i]["item_name"] << endl;
    }

}
catch (nlohmann::json::exception &e) {

    cout << "\nJSON Error: " << e.what() << endl;

}

        }
        else {
            cout << "\nGET Request Failed: "
                 << curl_easy_strerror(res) << endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}
bool save_lost_item(struct Details *p) {

    CURL *curl = curl_easy_init();

    if (!curl) {
        cout << "\nDatabase connection failed!";
        return false;
    }

    string API_KEY = "sb_publishable_UimWqc3sbw01H__BPeiFbg_MlMyntMg";

    struct curl_slist *headers = NULL;

    headers = curl_slist_append(headers,
        "Content-Type: application/json");

    string key_header = "apikey: " + API_KEY;

    headers = curl_slist_append(headers,
        key_header.c_str());

    string url =
    "https://ybykqrntfmruxyouzily.supabase.co/rest/v1/lost_items";

    string data =
    "{\"name\":\"" + json_escape(p->Name) +
    "\",\"student_id\":\"" + json_escape(p->ID) +
    "\",\"category\":\"" + json_escape(p->Category) +
    "\",\"item_name\":\"" + json_escape(p->Item_Name) +
    "\",\"brand_name\":\"" + json_escape(p->Brand_Name) +
    "\",\"item_color\":\"" + json_escape(p->Item_Color) +
    "\",\"unique_id\":\"" + json_escape(p->Unique_ID) +
    "\",\"last_seen_location\":\"" + json_escape(p->Last_Seen_Location) +
    "\",\"date_time\":\"" + json_escape(p->Date_Time) +
    "\",\"contact_info\":\"" + json_escape(p->Contact_Info) + "\"}";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    CURLcode result = curl_easy_perform(curl);

    long response_code = 0;

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,
                      &response_code);

    cout << "\nHTTP Code: " << response_code << endl;

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (result == CURLE_OK &&
        (response_code == 201 || response_code == 204)) {
        cout << "\nLost Item saved successfully!";
        return true;
    }

    cout << "\nError: " << curl_easy_strerror(result);
    return false;
}

void show_matching_items() {

    system("cls");

    cout << "\n============================================\n";
    cout << "             MATCHING RESULTS               \n";
    cout << "============================================\n";

    if (lost_items_data.empty() || found_items_data.empty()) {
        cout << "\nNo Lost or Found Items available.\n";
        getch();
        return;
    }

    const int MAX_MATCHES = 100;

    double match_percentages[MAX_MATCHES];
    int lost_indexes[MAX_MATCHES];
    int found_indexes[MAX_MATCHES];

    int match_count = 0;

    // Step 1: Collect ALL possible matches
    for (int i = 0; i < lost_items_data.size(); i++) {

        for (int j = 0; j < found_items_data.size(); j++) {

            double percentage = calculate_match_percentage(
                lost_items_data[i],
                found_items_data[j]
            );

            if (percentage >= 40 && match_count < MAX_MATCHES) {

                match_percentages[match_count] = percentage;
                lost_indexes[match_count] = i;
                found_indexes[match_count] = j;

                match_count++;
            }
        }
    }

    // Step 2: Global Bubble Sort
    for (int a = 0; a < match_count - 1; a++) {

        for (int b = 0; b < match_count - a - 1; b++) {

            if (match_percentages[b] < match_percentages[b + 1]) {

                double temp = match_percentages[b];
                match_percentages[b] = match_percentages[b + 1];
                match_percentages[b + 1] = temp;

                int temp_lost = lost_indexes[b];
                lost_indexes[b] = lost_indexes[b + 1];
                lost_indexes[b + 1] = temp_lost;

                int temp_found = found_indexes[b];
                found_indexes[b] = found_indexes[b + 1];
                found_indexes[b + 1] = temp_found;
            }
        }
    }

    // Step 3: Display ALL sorted matches

if (match_count == 0) {
    cout << "\nNo possible matches found.\n";
}

for (int k = 0; k < match_count; k++) {

    int i = lost_indexes[k];
    int j = found_indexes[k];

    double percentage = match_percentages[k];

    cout << "\n============================================\n";
    cout << "                 MATCH #" << k + 1 << endl;
    cout << "============================================\n";

    // LOST ITEM DETAILS
    cout << "\n---------- LOST ITEM DETAILS ----------\n";

    cout << "Name: "
         << get_json_field(lost_items_data[i], "full_name") << endl;

    cout << "Student ID: "
         << get_json_field(lost_items_data[i], "student_id") << endl;

    cout << "Item Name: "
         << get_json_field(lost_items_data[i], "item_name") << endl;

    cout << "Category: "
         << get_json_field(lost_items_data[i], "category") << endl;

    cout << "Brand: "
         << get_json_field(lost_items_data[i], "brand_name") << endl;

    cout << "Color: "
         << get_json_field(lost_items_data[i], "item_color") << endl;

    cout << "Unique ID: "
         << get_json_field(lost_items_data[i], "unique_id") << endl;

    cout << "Location: "
         << get_json_field(lost_items_data[i], "last_seen_location") << endl;

    cout << "Date & Time: "
         << get_json_field(lost_items_data[i], "date_time") << endl;

    cout << "Contact Info: "
         << get_json_field(lost_items_data[i], "contact_info") << endl;


    // FOUND ITEM DETAILS
    cout << "\n---------- FOUND ITEM DETAILS ----------\n";

    cout << "Name: "
         << get_json_field(found_items_data[j], "full_name") << endl;

    cout << "Student ID: "
         << get_json_field(found_items_data[j], "student_id") << endl;

    cout << "Item Name: "
         << get_json_field(found_items_data[j], "item_name") << endl;

    cout << "Category: "
         << get_json_field(found_items_data[j], "category") << endl;

    cout << "Brand: "
         << get_json_field(found_items_data[j], "brand_name") << endl;

    cout << "Color: "
         << get_json_field(found_items_data[j], "item_color") << endl;

    cout << "Unique ID: "
         << get_json_field(found_items_data[j], "unique_id") << endl;

    cout << "Location: "
         << get_json_field(found_items_data[j], "last_seen_location") << endl;

    cout << "Date & Time: "
         << get_json_field(found_items_data[j], "date_time") << endl;

    cout << "Contact Info: "
         << get_json_field(found_items_data[j], "contact_info") << endl;


    cout << "\nMatching Percentage: "
         << percentage << "%" << endl;

    cout << "--------------------------------------------\n";

    char choice;

    cout << "Send Contact Request for this match? (Y/N): ";
    cin >> choice;

    if (toupper(choice) == 'Y') {

        string lost_id =
            get_json_field(lost_items_data[i], "id");

        string found_id =
            get_json_field(found_items_data[j], "id");

        save_match_request(lost_id, found_id, percentage, LI_person.ID);
    }
}

    cout << "\nTotal Possible Matches: " << match_count << endl;

    cout << "\nPress any key to return to menu...";
    getch();
}

bool save_found_item(struct Details *p) {

    CURL *curl = curl_easy_init();

    if (!curl) {
        cout << "\nDatabase connection failed!";
        return false;
    }

    string API_KEY = "sb_publishable_UimWqc3sbw01H__BPeiFbg_MlMyntMg";

    struct curl_slist *headers = NULL;

    headers = curl_slist_append(headers,
        "Content-Type: application/json");

    string key_header = "apikey: " + API_KEY;

    headers = curl_slist_append(headers,
        key_header.c_str());

    string url =
    "https://ybykqrntfmruxyouzily.supabase.co/rest/v1/found_items";

    string data =
    "{\"name\":\"" + json_escape(p->Name) +
    "\",\"student_id\":\"" + json_escape(p->ID) +
    "\",\"category\":\"" + json_escape(p->Category) +
    "\",\"item_name\":\"" + json_escape(p->Item_Name) +
    "\",\"brand_name\":\"" + json_escape(p->Brand_Name) +
    "\",\"item_color\":\"" + json_escape(p->Item_Color) +
    "\",\"unique_id\":\"" + json_escape(p->Unique_ID) +
    "\",\"last_seen_location\":\"" + json_escape(p->Last_Seen_Location) +
    "\",\"date_time\":\"" + json_escape(p->Date_Time) +
    "\",\"contact_info\":\"" + json_escape(p->Contact_Info) + "\"}";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    CURLcode result = curl_easy_perform(curl);

    long response_code = 0;

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,
                      &response_code);

    cout << "\nHTTP Code: " << response_code << endl;

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (result == CURLE_OK &&
        (response_code == 201 || response_code == 204)) {

        cout << "\nFound Item saved successfully!";
        return true;
    }

    cout << "\nError: " << curl_easy_strerror(result);
    return false;
}

bool save_match_request(string lost_id, string found_id,
                        double percentage, string student_id) {

    CURL *curl = curl_easy_init();

    if (!curl) {
        cout << "\nDatabase connection failed!";
        return false;
    }

    string API_KEY = "sb_publishable_UimWqc3sbw01H__BPeiFbg_MlMyntMg";

    struct curl_slist *headers = NULL;

    headers = curl_slist_append(headers,
        "Content-Type: application/json");

    string key_header = "apikey: " + API_KEY;

    headers = curl_slist_append(headers,
        key_header.c_str());

    string url =
    "https://ybykqrntfmruxyouzily.supabase.co/rest/v1/match_requests";
    
    cout << "\nLost ID: " << lost_id;
    cout << "\nFound ID: " << found_id;

    string data =
    "{\"lost_item_id\":" + lost_id +
    ",\"found_item_id\":" + found_id +
    ",\"match_percentage\":" + to_string(percentage) +
    ",\"requester_student_id\":\"" + json_escape(student_id) +
    "\",\"status\":\"Pending\"}";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    CURLcode result = curl_easy_perform(curl);

    long response_code = 0;

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,
                      &response_code);

    cout << "\nHTTP Code: " << response_code << endl;

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (result == CURLE_OK &&
        (response_code == 201 || response_code == 204)) {

        cout << "\nContact Request Submitted Successfully!";
        return true;
    }

    cout << "\nRequest Failed!";
    return false;
}

void view_my_history() {

    system("cls");

    CURL *curl = curl_easy_init();

    if (!curl) {
        cout << "Database connection failed!";
        getch();
        return;
    }

    string response_body = "";

    struct curl_slist *headers = NULL;

    string API_KEY = "sb_publishable_UimWqc3sbw01H__BPeiFbg_MlMyntMg";
    string key_header = "apikey: " + API_KEY;

    headers = curl_slist_append(headers, key_header.c_str());

    string urls[] = {
        "https://ybykqrntfmruxyouzily.supabase.co/rest/v1/lost_items?select=*&student_id=eq." + LI_person.ID,
        "https://ybykqrntfmruxyouzily.supabase.co/rest/v1/found_items?select=*&student_id=eq." + LI_person.ID,
        "https://ybykqrntfmruxyouzily.supabase.co/rest/v1/match_requests?select=*&requester_student_id=eq." + LI_person.ID
    };

    string titles[] = {
        "MY LOST REPORTS",
        "MY FOUND REPORTS",
        "MY CONTACT REQUESTS"
    };

    for (int j = 0; j < 3; j++) {

        response_body = "";

        curl_easy_setopt(curl, CURLOPT_URL, urls[j].c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

        CURLcode result = curl_easy_perform(curl);

        cout << "\n\n========== " << titles[j] << " ==========\n";

        if (result == CURLE_OK) {

            nlohmann::json records =
                nlohmann::json::parse(response_body);

            cout << "Total Records: " << records.size() << endl;

            for (int i = 0; i < records.size(); i++) {

                cout << "\nRecord #" << i + 1 << endl;

                if (j == 0 || j == 1) {

                    cout << "Item Name: "
                         << get_json_field(records[i], "item_name") << endl;

                    cout << "Category: "
                         << get_json_field(records[i], "category") << endl;

                    cout << "Brand: "
                         << get_json_field(records[i], "brand_name") << endl;

                    cout << "Color: "
                         << get_json_field(records[i], "item_color") << endl;

                    cout << "Location: "
                         << get_json_field(records[i], "last_seen_location") << endl;

                    cout << "Date: "
                         << get_json_field(records[i], "date_time") << endl;
                }

                else {

                    cout << "Lost Item ID: "
                         << get_json_field(records[i], "lost_item_id") << endl;

                    cout << "Found Item ID: "
                         << get_json_field(records[i], "found_item_id") << endl;

                    cout << "Match Percentage: "
                         << get_json_field(records[i], "match_percentage") << "%" << endl;

                    cout << "Status: "
                         << get_json_field(records[i], "status") << endl;
                }
            }
        }

        else {
            cout << "Request failed!";
        }
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    cout << "\n\nPress any key...";
    getch();
}

int main() {


    int menu_option;
    int display_details_option;
    bool is_first_launch = false;
    bool FI_saved = false;

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

    if (LI_person.ID.empty() || LI_person.ID.length() < 8) {
        cout << endl << " Error: Invalid ID.Please try again" << endl << endl;
        cout << " Enter ID: ";
        getline(cin, LI_person.ID);
    }
    }//end of while()
     
    FI_person.Name = LI_person.Name;
    FI_person.ID   = LI_person.ID;
     


    directing();

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
            cout << "\t\t\t\t\t\t    [5] My History" << endl << endl;
            cout << "\t\t\t\t\t\t    [6] Exit" << endl << endl;

            cout << endl << "\t\t\t\t\t\t    Enter option : ";
            menu_option = getche();

            if (menu_option == EXIT) {
                cout << endl << endl << endl << " Exited successfully...";
                return 0;
            }

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
						cout << endl << endl << "  Error: Invalid Option" << endl;
					}
				}while(display_details_option != LOST_ITEM && display_details_option != FOUND_ITEM);
            }// end of outer if()

            if (menu_option == LOST_ITEM || menu_option == FOUND_ITEM || menu_option == SEARCH_ITEM || menu_option == DISPLAY_DETAILS || menu_option == MY_HISTORY) {
                break;
            }

        }while (menu_option != LOST_ITEM && menu_option != FOUND_ITEM && menu_option != SEARCH_ITEM && menu_option != DISPLAY_DETAILS && menu_option != MY_HISTORY && menu_option != EXIT );

        system("cls");

        switch (menu_option) {


            case LOST_ITEM:

                cout << endl << endl << "  Report Lost Item" << endl;
                cout << "----------------------------------" << endl << endl << endl;
                display_LI_inputs(&LI_person);

                    if (!is_LI_input_blank(&LI_person)) {
                        cout << endl << "  All details are provided.";
                        
                        save_lost_item(&LI_person);
                    }
                    else{
                    	cout << endl << "Please complete All details.";
					}

                break;



            case FOUND_ITEM:
                cout << endl << endl << "  Report Found Item" << endl;
                cout << "----------------------------------" << endl << endl << endl;

                    display_FI_inputs(&FI_person);

                    if (!is_FI_input_blank(&FI_person)) {
                        cout << endl <<  "  All details are provided.";
                        if(!FI_saved){
                        	
                        	FI_saved =
                        	save_found_item(&FI_person);
						}
						else{
							cout << endl << "This Found Item is already save.";
						}
                    }
                    else {
                    	cout << endl << "Please complete All details.";
					}
                break;

            case SEARCH_ITEM:
                cout << endl << endl << "  Search Item" << endl;
                cout << "----------------------------------";
                get_lost_items();
                get_found_items();
                
                show_matching_items();
                
                break;
                
                case DISPLAY_DETAILS:

                if (display_details_option == LOST_ITEM) {

                cout << endl << endl << "  Lost Item Details" << endl;
                cout << "----------------------------------" << endl << endl << endl;
                cout << "   Name                : " << LI_person.Name << endl;
                cout << "   ID                  : " << LI_person.ID << endl;
                cout << "   Category            : " << LI_person.Category << endl;
                cout << "   Item name           : " << LI_person.Item_Name << endl;
                cout << "   Brand name          : " << LI_person.Brand_Name << endl;
                cout << "   Item color          : " << LI_person.Item_Color << endl;
                cout << "   Unique ID           : " << LI_person.Unique_ID << endl;
                cout << "   Last seen location  : " << LI_person.Last_Seen_Location << endl;
                cout << "   Date time           : " << LI_person.Date_Time << endl;
                cout << "   Contact info        : " << LI_person.Contact_Info << endl;

                }

                if (display_details_option == FOUND_ITEM) {

                cout << endl << endl << "  Found Item Details" << endl;
                cout << "----------------------------------" << endl << endl << endl;
                cout << "   Name                : " << FI_person.Name << endl;
                cout << "   ID                  : " << FI_person.ID << endl;
                cout << "   Category            : " << FI_person.Category << endl;
                cout << "   Item name           : " << FI_person.Item_Name << endl;
                cout << "   Brand name          : " << FI_person.Brand_Name << endl;
                cout << "   Item color          : " << FI_person.Item_Color << endl;
                cout << "   Unique ID           : " << FI_person.Unique_ID << endl;
                cout << "   Last seen location  : " << FI_person.Last_Seen_Location << endl;
                cout << "   Date time           : " << FI_person.Date_Time << endl;
                cout << "   Contact info        : " << FI_person.Contact_Info << endl;

                }
                
                break;
                
            case MY_HISTORY:
                view_my_history();
                break;
                	


        }//end of switch
         
        cout << endl << endl << "  Press any key... ";
        (void)getch();
        is_first_launch = true;
         

    }while (1);

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

bool is_LI_input_blank(struct Details *LI_person) {

    if(LI_person->Category.empty() || LI_person->Item_Name.empty() || LI_person->Brand_Name.empty() ||
       LI_person->Item_Color.empty() || LI_person->Unique_ID.empty() || LI_person->Last_Seen_Location.empty() ||
       LI_person->Date_Time.empty() || LI_person->Contact_Info.empty()) {
        return true;
    }

    else {
        return false;
    }
}

bool is_FI_input_blank(struct Details *FI_person) {

    if(FI_person->Category.empty() || FI_person->Item_Name.empty() || FI_person->Brand_Name.empty() ||
       FI_person->Item_Color.empty() || FI_person->Unique_ID.empty() || FI_person->Last_Seen_Location.empty() ||
       FI_person->Date_Time.empty() || FI_person->Contact_Info.empty()) {
        return true;
    }

    else {
        return false;
    }
}



void display_LI_inputs(struct Details *LI_person) {

    /* if (is_lost_active) { */

        if (LI_person->Category.empty()) {
                cout << "  For Example: [Electronics | Documents | Apparel | Valuables | Other]" << endl << endl; 
                cout << "  Enter Item Category: ";
                getline(cin, LI_person->Category);
        }

         if (LI_person->Item_Name.empty()) {
                cout << endl << "  Enter Item Name: ";
                getline(cin, LI_person->Item_Name);
        }

         if (LI_person->Brand_Name.empty()) {
                cout << endl << "  Enter Brand Name: ";
                getline(cin, LI_person->Brand_Name);
        }

         if(LI_person->Item_Color.empty()) {
                cout << endl << "  Enter Color: ";
                getline(cin, LI_person->Item_Color);
        }

         if (LI_person->Unique_ID.empty()) {
                cout << endl << "  For Example: [Serial # | Stickers | dents | Custom marks | Other]" << endl; 
                cout << endl << "  Enter Unique Identifiers: ";
                getline(cin, LI_person->Unique_ID);
        }

         if (LI_person->Last_Seen_Location.empty()) {
                cout << endl << "  Enter Last Seen Location: ";
                getline(cin, LI_person->Last_Seen_Location);
        }

         if (LI_person->Date_Time.empty()) {
                cout << endl << "  Enter Date and Time (DD-MM-YYYY HH:MM AM/PM): ";
                getline(cin, LI_person->Date_Time);
        }

         if (LI_person->Contact_Info.empty()) {
                cout << endl << "  Enter Contact Info \n(For Example: [Phone Number | Gmail | Other]): ";
                getline(cin, LI_person->Contact_Info);
         }


    /* }//end of outer if() */
     
     
}


void display_FI_inputs(struct Details *FI_person) {

    /* if (is_found_active) { */

        if (FI_person->Category.empty()) {
                cout << "  For Example: [Electronics | Documents | Apparel | Valuables | Other]" << endl << endl; 
                cout << "  Enter Item Category: ";
                getline(cin, FI_person->Category);
        }

         if (FI_person->Item_Name.empty()) {
                cout << endl << "  Enter Item Name: ";
                getline(cin, FI_person->Item_Name);
        }

         if (FI_person->Brand_Name.empty()) {
                cout << endl << "  Enter Brand Name: ";
                getline(cin, FI_person->Brand_Name);
        }

         if(FI_person->Item_Color.empty()) {
                cout << endl << "  Enter Color: ";
                getline(cin, FI_person->Item_Color);
        }

         if (FI_person->Unique_ID.empty()) {
                cout << endl << "  For Example: [Serial # | Stickers | dents | Custom marks | Other]" << endl; 
                cout << endl << "  Enter Unique Identifiers: ";
                getline(cin, FI_person->Unique_ID);
        }

         if (FI_person->Last_Seen_Location.empty()) {
                cout << endl << "  Enter Last Seen Location: ";
                getline(cin, FI_person->Last_Seen_Location);
        }

         if (FI_person->Date_Time.empty()) {
                cout << endl << "  Enter Date and Time (DD-MM-YYYY HH:MM AM/PM): ";
                getline(cin, FI_person->Date_Time);
        }

         if (FI_person->Contact_Info.empty()) {
                cout << endl << "  Enter Contact Info \n(For Example: [Phone Number | Gmail | Other]): ";
                getline(cin, FI_person->Contact_Info);
         }

}

void display_details(struct Details *p){
	
	cout << "    Name            :" << p->
	Name << endl;
		cout << "    ID           :" << p->
	ID << endl;
		cout << "    Category            :" << p->
	Category << endl;
		cout << "    Item name           :" << p->
	Item_Name << endl;
		cout << "    Brand name            :" << p->
	Brand_Name << endl;
		cout << "    Item color            :" << p->
	Item_Color << endl;
		cout << "    Unique ID            :" << p->
	Unique_ID << endl;
     	cout << "    Last seen location           :" << p->
	Last_Seen_Location<< endl;
	    cout << "    Data time            :" << p->
	Date_Time << endl;
       	cout << "    Contact info            :" << p->
	Contact_Info << endl;
}

