    #include <iostream>
    #include <string>
    #include <unordered_map>
    #include <functional>
    #include <utility>
    #include <cstdlib>
    #include <stack>
    #include <vector>

    //Hash table stores user login info
    //Graph stores each game by genre
    //BST stores what games each friend owns 
    //Linked list stores a list of games reccomended from the users preferred genre 
    //Stack stores adds undo functionality 

    using namespace std;

    void clearConsole() {
    #ifdef _WIN32 
        system("cls");
    #else 
        std::cout << "\033[2J\033[1;1H";
    #endif
    }

    enum LOCATION{
        Steam,
        Nintendo,
        PlayStation,
        Xbox
    };

    enum GENRE{
        Horror,
        Action,
        Shooter,
        Platformer
    };

    enum ACTIONTYPE{
        addGame,     // 1
        removeGame,  // 2
        addFriend,   // 3
        removeFriend,// 4
    };

    struct Game{
        string Title;
        GENRE Genre;
        LOCATION Location;
    };

    struct Friend{
        string Name;
    };

    struct ActionT {
        ACTIONTYPE type;
       
        Game game;
        
        Friend frien;
        
    };

    class GameGraph{
        public:
        vector<vector<Game>> graph;
        GameGraph() : graph(4) {};

        void addGame(const Game& g){
            graph[g.Genre].push_back(g);
        }

        Game removeGame(const string& title) {
        Game copy;
        for (auto &genreList : graph) {
            for (auto it = genreList.begin(); it != genreList.end(); ++it) {
                if (it->Title == title) {
                    copy.Genre = it->Genre;
                    copy.Title = it->Title;
                    copy.Location = it->Location;
                    genreList.erase(it);
                    return copy;
                }
            }
        }
    }
    void printLibrary() const {
        for (int g = Horror; g <= Platformer; ++g) {
            cout << "Genre: ";
            switch(static_cast<GENRE>(g)) {
                case Horror:     cout << "Horror"; break;
                case Action:     cout << "Action"; break;
                case Shooter:    cout << "Shooter"; break;
                case Platformer: cout << "Platformer"; break;
            }
            cout << "\n";

            if (graph[g].empty()) {
                cout << "  (no games)" << endl;
            } else {
                for (const auto &game : graph[g]) {
                    cout << "  - " << game.Title << " (";
                    switch (game.Location) {
                        case Steam:      cout << "Steam"; break;
                        case Nintendo:   cout << "Nintendo"; break;
                        case PlayStation:cout << "PlayStation"; break;
                        case Xbox:       cout << "Xbox"; break;
                    }
                    cout << ")" << endl;
                }
            }
            cout << "\n";
        }
    }

    void simplePrint() const {
    bool first = true;
    for (const auto& genreList : graph) {
        for (const auto& game : genreList) {
            if (!first) cout << ", ";
            cout << game.Title;

            first = false;
            }
        }
        cout << endl;
    }

    void simplePrintNumbered() const {
    int i = 1;
    bool first = true;
    for (const auto& genreList : graph) {
        for (const auto& game : genreList) {
            if (!first) cout << ", ";
            cout << "(" << i << ")" << " " << game.Title;
            i++;
            first = false;
            }
        }
        cout << endl;
    }

    const vector<Game>& gamesOfGenre(GENRE g) const {
        return graph[g];
    }
};

    //Dual purpose (next for linked list, right/left for BST)
    class Node{
        public:
        Game game;
        Node* next = nullptr;

        Node* right = nullptr; //Owns game
        Node* left = nullptr;  //Doesn't own game

        Node(Game inGame){
            game = inGame;
        }
    };

    Node* genreRec(const GameGraph &gG, GENRE pref) {
        if(gG.gamesOfGenre(pref).empty()){
            return nullptr;
        }
        Node* head = nullptr;
        Node* tail = nullptr;

    const vector<Game>& games = gG.gamesOfGenre(pref);
    for (const auto& game : games) {
        Node* newNode = new Node(game);

        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }
    return head; 
}

    void printRecList(Node* list){
        for(Node* cur = list; cur != nullptr; cur = cur->next){
            cout << cur->game.Title << ", ";
        }
        cout << endl;
    }

    //Hash table for storing user login 
    unordered_map<string, size_t> users;

    size_t hash_password(const string &pass){
        hash<string> hasher;
        return hasher(pass);
    }

    void addUser(string userN, string passW){
        users[userN] = hash_password(passW);
    }



    class friendBST{
        Node* rightRoot = nullptr;
        Node* leftRoot = nullptr;;

        Node* insertRec(Node* node, const Game& g) {
        if (!node){
            return new Node(g);
        } 
        if (g.Title < node->game.Title){
            node->left = insertRec(node->left, g);
        }
            
        else if (g.Title > node->game.Title){
             node->right = insertRec(node->right, g);
        }
        return node;
    }

        void inOrder(Node* node) const {
            if (!node){ 
            return;
        }
            inOrder(node->left);
            cout << "  - " << node->game.Title << endl;
            inOrder(node->right);
    }
        Friend friend_;
        public:
        string getN(){
            return friend_.Name;
        }
        friendBST(Friend fren, GameGraph g){
            friend_ = fren;
            vector<int> inputList;

            vector<Game*> allGames;

            for(auto &genreList : g.graph){
                for(auto &game : genreList){
                    allGames.push_back(&game);
                }
            }

            clearConsole();
            int gameCount;
            cout << "How many games does " << fren.Name << " own: " << endl;
            cin >> gameCount;
            cout << "What games does " << fren.Name << " own: (enter each digit and then press enter)" <<endl;
            g.simplePrintNumbered();

            vector<bool> owns(allGames.size() + 1, false); // 1-based
                for (int j = 0; j < gameCount; ++j) {
                    int idx;
                    cin >> idx;
                    if (idx >= 1 && idx <= (int)allGames.size()) {
                        owns[idx] = true;
                }else{
                cout << "Invalid index " << idx << ", skipping.\n";
                }
            }

            int i = 1;
            for (Game* gp : allGames) {
                if (owns[i]) {
                rightRoot = insertRec(rightRoot, *gp);   // OWNED → right subtree
                }else{
                leftRoot = insertRec(leftRoot, *gp);     // NOT OWNED → left subtree
            }
            ++i;
        }
        cout << endl;
        };
        void printOwned() const {
        cout << friend_.Name << " OWNS:\n";
        inOrder(rightRoot);
    }
        
    };

    int main(){
        //STACK FOR STORING LAST ACTION
        stack<ActionT> stk;
        //Graph for storing games
        GameGraph gGraph;

        //BST for storing friends
        vector<friendBST> friendList;
        
        clearConsole();
        
        //-----------------------------------------------------------
        //If hash table is empty add a new user
        if(users.size() == 0){
            cout << "--------No Accounts Found, Please Create an account--------------" << endl;
            string user;
            cout << "Enter Username: ";
            cin >> user;

            string pass;
            cout << "Enter Password: ";
            cin >> pass;

            addUser(user, pass);
            pass = " ";

            clearConsole();
        }

        //User login 
        cout << "--------------Please Log In--------------" << endl;

        string user;
        cout << "Username: ";
        cin >> user;

        auto it = users.find(user);
        if(it == users.end()){
            cout << "User not found";
            return 0;
        }

        string pass;
        cout << "Password: ";
        cin >> pass;

        if(hash_password(pass) != it->second){
            cout << "Please try again....";
            return 0;
        }
        clearConsole();
        
        //-----------------------------------------------------------

        //Get user preference 
        int prefIn = 0;
        GENRE userPref;
        cout <<"Choose Genre Preference: " <<endl;
        cout <<"1 HORROR" << endl;
        cout <<"2 ACTION" << endl;
        cout <<"3 SHOOTER" << endl;
        cout <<"4 PLATFORMER" << endl;

        cin >> prefIn;

        switch(prefIn){
            case 1:
            userPref = Horror;
            break;
            case 2:
            userPref = Action;
            break;
            case 3:
            userPref = Shooter;
            break;
            case 4:
            userPref = Platformer;
            break;
        }

        while(true){
        clearConsole();
        cout <<"--------------------" << endl;
        cout <<"Preference: ";
        if(userPref == Horror){
            cout << "Horror" << endl;
        }else if(userPref == Action){
            cout << "Action" << endl;
        }else if(userPref == Shooter){
            cout << "Shooter" << endl;
        }else if(userPref == Platformer){
            cout << "Platformer" << endl;
        }
        cout <<"1 ADD GAME" << endl;
        cout <<"2 ADD FRIEND" << endl;
        cout <<"3 VIEW LIBRARY" << endl;
        cout <<"4 REMOVE GAME" << endl;
        cout <<"5 REMOVE FRIEND" << endl;
        cout <<"6 GET RECOMMENDATION FROM FAVORITE GENRE" << endl;
        cout <<"7 SHOW FRIEND(S)" << endl;
        cout <<"8 UNDO LAST ACTION: " << endl;
        if(stk.size() == 0){
            cout << "NO ACTION TO UNDO" << endl;
        }else{
            if (stk.top().type == addGame){
                cout << "ADDED " << stk.top().game.Title << endl;
            }else if(stk.top().type == removeGame){
                cout << "REMOVED " << stk.top().game.Title << endl;
            }else if(stk.top().type == addFriend){
                cout << "ADDED " << stk.top().frien.Name << " AS A FRIEND" << endl;
            }else if(stk.top().type == removeFriend){
                cout << "REMOVED " << stk.top().frien.Name << " AS A FRIEND" << endl;
            }
        }   
        cout <<"--------------------" << endl;

        int input = 0;
        cin >> input;

        switch(input){
            case 1: {
            string title;
            int genre = 0;
            int location = 0;
            cout <<"---ADD GAME---" << endl;
            cout <<"Enter Title: "; cin >> title;
            cout <<"Enter Genre: " << endl << "1. Horror | 2. Action | 3. Shooter | 4. Platformer" << endl; cin >> genre;
            cout <<"Enter Location: " << endl << "1. Steam | 2. Nintendo | 3. PlayStation | 4. Xbox" << endl; cin >> location;

            Game newGameAdded;
            newGameAdded.Title = title;

            //Could use another switch but a switch inside a switch looks funny to me lol 
            if(genre == 1){
                newGameAdded.Genre = Horror;
            }else if(genre == 2){
                newGameAdded.Genre = Action;
            }else if(genre == 3){
                newGameAdded.Genre = Shooter;
            }else if(genre == 4){
                newGameAdded.Genre = Platformer;
            }else{
                cout <<"Invalid Input";
            }

            if(location == 1){  
                newGameAdded.Location = Steam;
            }else if(location == 2){
                newGameAdded.Location = Nintendo;
            }else if(location == 3){
                newGameAdded.Location = PlayStation;
            }else if(location == 4){
                newGameAdded.Location = Xbox;
            }else{
                cout <<"Invalid Input";
            }
            
            gGraph.addGame(newGameAdded);

            //For undo stuff
            ActionT action;
            action.type = addGame;
            action.game = newGameAdded;
            stk.push(action);
            break;
            }
            //ADD FRIEND
            case 2:{
                Friend buddy;
                string name;
                cout << "Enter your friend's name: " << endl;
                cin >> name;
                buddy.Name = name;

                friendList.emplace_back(buddy, gGraph);



                ActionT newFriend;
                newFriend.type = addFriend;
                newFriend.frien = buddy;
                stk.push(newFriend);
                break;
            }
            break;
            case 3:
            int wait;
            gGraph.printLibrary();
            cin >> wait;
            break;
            case 4:{
            clearConsole();
            Game removedGame;
            string gameToBeRemoved;
            gGraph.simplePrint();
            cout << "Enter the game to be removed: " << endl;
            cin >> gameToBeRemoved;
            removedGame = gGraph.removeGame(gameToBeRemoved);
            
            ActionT action;
            action.type = removeGame;
            action.game = removedGame;
            stk.push(action);
            break;
            }

            //Remove friend
            case 5:{
                Friend removedFriend;   
                int in;
                if(friendList.empty()){
                    cout << "No Friends to Remove" << endl;
                    cin >> in;
                    break;
                }
                
                cout << "Select friend to remove" << endl;
                int j = 1;
                for(auto i : friendList){
                    cout << "(" << j << ") " << friendList[j-1].getN() <<  " ";
                    j++;
                } cout << endl;

                cin >> in;

                removedFriend.Name = friendList[in-1].getN();
                friendList.erase(friendList.begin() + in - 1);
                ActionT action;
                action.type = removeFriend;
                action.frien = removedFriend;
                stk.push(action);
                break;
            }
            
            
            case 6:{
                Node* input = genreRec(gGraph, userPref);
                if(input == nullptr){
                    cout << "No games found!" << endl;
                }
                printRecList(input);
                int in; cin >> in;
                break;
            }
            case 7:{
                int in;
                if(friendList.empty()){
                    cout << "No friends to view" << endl;
                    cin >> in;
                    break;
                }

                cout << "Select friend to view" << endl;
                int j = 1;
                for(auto i : friendList){
                    cout << "(" << j << ") " << friendList[j-1].getN() <<  " ";
                    j++;
                } cout << endl;

                cin >> in;

                friendList[in-1].printOwned();

                cin >> in;
            break;
            }      
            case 8:
            if(stk.empty() == 1){
                break;
            }
            if(stk.top().type == addGame){
                gGraph.removeGame(stk.top().game.Title);
                stk.pop();
            }else if(stk.top().type == removeGame){
                gGraph.addGame(stk.top().game);
                stk.pop();
            }else if(stk.top().type == addFriend){
                int j = 0;
                for(auto i : friendList){
                    if(friendList[j].getN() == stk.top().frien.Name){
                        friendList.erase(friendList.begin() + j);
                    }
                }
                stk.pop();
            }else if(stk.top().type == removeFriend){
                friendList.emplace_back(stk.top().frien, gGraph);
                stk.pop();
            }
            break;
        }
        }
    }