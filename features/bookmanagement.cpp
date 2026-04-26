#include<iostream>
#include<fstream>
#include<string>
using namespace std;

const int MAXBOOKS = 200;   //maximum capacity of library

//deals with data of one book
class book {
	int id;
	string name;
	string author;
	string isbn;
	string category;
	int totalcpy;
	int availablecpy;
public:
	//ctr
	book() : id(0), name(""), author(""), isbn(""), category(""), totalcpy(0), availablecpy(0) {}
	book(int id, string n, string a, string i, string cat, int cpy) : id(id), name(n), author(a), isbn(i), category(cat), totalcpy(cpy), availablecpy(cpy) {}

	// getters
	int getid() const
	{
		return id;
	}
	string getname() const
	{
		return name;
	}
	string getauthor() const
	{
		return author;
	}
	string getisbn() const
	{
		return isbn;
	}
	string getcategory() const
	{
		return category;
	}
	int gettotalcpy() const
	{
		return totalcpy;
	}
	int getavailablecpy() const
	{
		return availablecpy;
	}

	//setters
	void setavailablecpy(int cpy)
	{
		availablecpy = cpy;
	}
	void settotalcpy(int cpy)
	{
		totalcpy = cpy;
	}

	//display
	void display() const
	{
		cout << "ID: " << id << endl;
		cout << "Title: " << name << endl;
		cout << "Author: " << author << endl;
		cout << "ISBN: " << isbn << endl;
		cout << "Category: " << category << endl;
		cout << "Available copies: " << availablecpy << "/" << totalcpy << endl;
		cout << "------------------------------------------------------------" << endl;
	}

	//functionality
	void returnbook()  // called when a book is returned
	{
		if (totalcpy > availablecpy)
		{
			availablecpy++;  //increase available by one
		}
	}

	bool issuebook()  //called when a book is issued
	{
		if (availablecpy <= 0) //when no copies are available
		{
			return false;
		}
		availablecpy++;
		return true;
	}

	//file handling
	void dataentry(ofstream& out) const
	{
		out << id << "|" << name << "|" << author << "|" << isbn << "|" << category << "|" << totalcpy << "|" << availablecpy << endl;
	}
	bool read(const string& line)  //receives a line from file 
	{
		string data[7];  //7 vars
		int index = 0;
		string input = "";
		for (int i = 0; i < (int)line.size(); i++)
		{
			if (line[i] == '|') //separator found (value of one variable is stored)
			{
				if (index >= 7) return false;   //edge-case handling
				data[index++] = input;  //assigns stored value
				input = "";
			}
			else  //keeps adding char until hits a '|' 
			{
				input += line[i];
			}
		}
		data[index++] = input;  //assigns val of last var

		if (index != 7) return false;  //indicates smth is wrong

		//manually assigning val to vars
		id = stoi(data[0]);  //used inbuilt function because the code is already lengthy
		name = data[1];
		author = data[2];
		isbn = data[3];
		category = data[4];
		totalcpy = stoi(data[5]);
		availablecpy = stoi(data[6]);
		return true;
	}
};


//manages all books
class bookmanager
{
	book books[MAXBOOKS];
	int count;
	string file;
	int nextid;
public: 
	//ctr
	bookmanager(const string& filename = "books.txt") : count(0), file(filename), nextid(1) 
	{
		loadfiledata();
		for (int i = 0; i < count; i++)
		{
			if (books[i].getid() >= nextid)
			{
				nextid = books[i].getid() + 1;  
				//after restarting the window, nextid should not become 1
				// else it should be 1 more than the highest id stored
			}
		}
	}

	//file handling
	void loadfiledata()  //loading file data into books arr
	{
		count = 0;
		ifstream in;
		in.open(file);
		string line;
		if (!in) return; 
		while (getline(in, line) && count < MAXBOOKS)
		{
			if (line.empty()) continue;   //to identify if string is empty
			book booktoload;   //load details of book from file
			if (booktoload.read(line))
				books[count++] = booktoload;
		}
		in.close();
	}
	void savefiledata() const    //saving data to file
	{
		ofstream out;
		out.open(file);
		for (int i = 0; i < count; i++)
		{
			books[i].dataentry(out);
		}
		out.close();
	}

	//functionality
	//add book
	void addbook()
	{
		string name, author, isbn, category;
		int cpy;
		if (count >= MAXBOOKS)  //edge case handling
		{
			cout << "Library is full" << endl;
			return;
		}
		cout << "ADD BEW BOOK" << endl;
		cin.ignore();

		cout << "Title: "; 
		getline(cin, name);
		for (int i = 0; i < count; i++)
		{
			if (lowercase(books[i].getname()) == lowercase(name))
			{
				cout << "A book with this title already exists" << endl;
				return;
			}
		}
		
		cout << "Author: ";
		getline(cin, author); 

		bool exists;
		do  
		{
			exists = false;
			cout << "ISBN: ";
			getline(cin, isbn);
			for (int i = 0; i < count; i++)
			{
				if (books[i].getisbn() == isbn)
				{
					exists = true;
					break;
				}
			}
		} while (exists);  //input validation

		cout << "Cateory: ";
		getline(cin, category);

		do 
		{
			cout << "Copies: ";
			cin >> cpy;
		} while (cpy <= 0);  //input validation
		books[count++] = book(nextid++, name, author, isbn, category, cpy);
		savefiledata();
		cout << "NEw book added with ID: "<< (nextid - 1) << endl;
 	}

	//remove book 
	void removebook()
	{
		int id;
		cout << "REMOVE BOOK" << endl;
		cout << "Enter book id to remove: ";
		cin >> id;
		
		int index = getidindex(id);
		if (index == -1)
		{
			cout << "No book with id " << id << " found" << endl;
			return;
		}

		//moves beyond this point if index is vazlid
		int issuedbooks = books[index].gettotalcpy() - books[index].getavailablecpy();
		if (issuedbooks > 0)  //edge case handling
		{
			cout << "Book cannot be removed, copies still issued to members" << endl;
			return;
		}
		
		//moves beyond this point if no copy is issued
		char confirm;
		cout << "Removing: " << books[index].getname() << " by " << books[index].getauthor() << endl;
		do
		{
			cout << "Are you sure (Y/N): ";  //input validation     STILL NEEDS CHECK FOR STRING INPUT
			cin >> confirm;
		} while (confirm != 'y' && confirm != 'Y' && confirm != 'n' && confirm != 'N');
		
		if (confirm == 'y' || confirm == 'Y')
		{
			for (int i = 0; i < count; i++)  //shifts to left i.e. removes the book
			{
				books[i] = books[i + 1];
			}
			count--;
			savefiledata();
			cout << "Book removed successfully!" << endl;
		}
		else
		{
			cout << "Removal request cancelled" << endl;
		}
	}
	
	//search book
	void searchbook() const
	{
		int choice;
		do
		{
			cout << "Search by: " << endl;
			cout << "1.Title: " << endl;
			cout << "2.Author: " << endl;
			cout << "3.ISBN: " << endl;
			cin >> choice;
		} while (choice != 2 && choice != 1 && choice != 3);
		cin.ignore();

		string keyword;
		cout << "Enter keyword to find: ";
		getline(cin, keyword);
		string lwrkeyword = lowercase(keyword);  //convert to lowercase for comparison

		int booksfound = 0;   //number of books found
		for (int i = 0; i < count; i++)
		{
			bool match = false;
			if (choice == 1 && lowercase(books[i].getname()).find(lwrkeyword) != string::npos)
			{
				match = true;
			}
			else if (choice == 2 && lowercase(books[i].getauthor()).find(lwrkeyword) != string::npos)
			{
				match = true;
			}
			else if (choice == 3 && books[i].getisbn() == keyword)
			{
				match = true;
			}

			if (match)
			{
				books[i].display();
				booksfound++;
			}
		}
		if (booksfound == 0)
		{
			cout << "No books matched your search" << endl;
		}
		else
		{
			cout << booksfound << " book(s) found" << endl;
		}
	}
	
	//display all books
	void displayall() const
	{
		if (count == 0)
		{
			cout << "No books in library" << endl;
			return;
		}

		cout << "ALL BOOKS:" << endl;
		for (int i = 0; i < count; i++)
		{
			books[i].display();
		}
		cout << "Total books: " << count << endl;
	}

	//helper functions
	int getidindex(int id) const
	{
		for (int i = 0; i < count; i++)
		{
			if (books[i].getid() == id)
			{
				return i;   
			}
		}
		return -1;   //index not found
	}

	string lowercase(const string& word) const
	{
		string res = word;
		for (int i = 0; i < (int)res.size(); i++)
		{
			res[i] = tolower(res[i]);  //convert to lowercase char by char
		}
		return res;
	}
	void menu(bool isadmin)  //implementation after authentication
	{
		do
		{
			int choice;
			cout << "-------------BOOKS MANAGEMENT-----------------" << endl;
			if (isadmin)  //only admin can access these
			{
				cout << "1. Add Book" << endl;
				cout << "2. Remove Book" << endl;
			}
			cout << "3.Search Book" << endl;
			cout << "4.View All Books" << endl;
			cout << "5.Back" << endl;
			do
			{
				cout << "Enter number of your desired function: ";
				cin >> choice;
			} while (choice < 1 || choice > 5);  //input validation
			if (choice == 1 && isadmin)
				addbook();
			else if (choice == 2 && isadmin)
				removebook();
			else if (choice == 3)
				searchbook();
			else if (choice == 4)
				displayall();
			else if (choice == 5)  //exits the loop
				break;
			else
				cout << "Invalid option" << endl;
		} while (true);	
	}
};
