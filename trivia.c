/*
Names:          Gavin Sereg     &     AJ Sauberlich

Date:           4/11/24

Description:    This is a terminal oriented trivia game. The user will be prompted to answer a question. If 
                they get the question right then their score will be incremented. If the user gets the question wrong then their score will remain the same. The game will prompt the user to play another round and will run until the user chooses to quit.
                
                Additionally the user's scores will be recorded in another CSV file, with the date, and the user may choose to view them in the menu. The program gets data from the CSV file. It uses a random number generator to pick a random question based off the random number and ID number from the CSV questions.

IMPORTANT:      Make sure that the questions file is a CSV not a TXT file, program will not properly excecute if 
                the file is a TXT.
                
                Compile command: 'gcc -o trivia trivia.c'

                Run command: './trivia'

                Everything is well documented with comments and headers on the functions describing their responsibilities.
*/


/*----------------------------------|
|              Includes             |
|----------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


/*----------------------------------|
|              Defines              |
|----------------------------------*/

#define QUESTIONS 20


/*----------------------------------|
|              Structs              |
|----------------------------------*/

/*
    This is a struct to hold the data from the CSV file. The id will be used to identify a certain question and answer.
*/
typedef struct{
    int id;
    char* question;
    char* answer;
}CSVData;


/*----------------------------------|
|            Prototypes             |
|----------------------------------*/

void displayMenu(int* ptrOption);

void getMenuOption(int* ptrRunning, int* ptrOption, int score);

int randomNumberGen();

void getDataFromCSV(CSVData* dataHolder, int randomNumber);

void askQuestion(CSVData* dataHolder);

void grade(CSVData* dataHolderCopy, int* userScore);

int playAgain();

void endGame(int* ptrRunning, int* ptrScore);

void writeData(int* ptrScore);

void viewScores();


/*----------------------------------|
|            Main Function          |
|----------------------------------*/

/*
    The main function will run the game loop and take care of exiting the program.
*/
int main(int argc, char const *argv[])
{
    // Clear window for cleaner look
    system("clear");

    // Garuntee that we get a random number each time
    srand(time(NULL));

    // Variables
    int option = 0;
    int running = 0;
    int score = 0;
    int userQuitGame = 0;
    int randomNumber = 0;

    // Create a struct to hold the data from the CSV
    CSVData dataHolder;

    // Allocate memory for the struct attributes
    dataHolder.question = (char*) malloc(sizeof(char) * 150);
    dataHolder.answer = (char*) malloc(sizeof(char) * 50);

    // Start the menu loop 
    while(option != 3){
        
        // Clear window to view the quesiton
        system("clear");

        // Display the menu
        displayMenu(&option);

        // Get the user's option choice
        getMenuOption(&running, &option, score);

        // Play game if the user chose to play
        while(running)
        {   
            // Clear window to view the quesiton
            system("clear");

            // Get a random number from 1 - 10
            randomNumber = randomNumberGen();
            
            // Get a record from the CSV file to store in dataHolder
            getDataFromCSV(&dataHolder, randomNumber);

            // Ask the user a random question
            askQuestion(&dataHolder);

            // Determine if they got it right or wrong
            grade(&dataHolder, &score);

            // See if they would like to play again
            userQuitGame = playAgain();

            // Clear window for new question or to print results after ending the game
            system("clear");
            
            // If user ends the game then print their score and append the score to the CSV
            if (!userQuitGame){
                endGame(&running, &score);
                break;
            }
        }
    }

    // Free the memory of dataHolders attributes before exiting.
    free(dataHolder.question);
    free(dataHolder.answer);

    // Exit program
    return 0;
}


/*----------------------------------|
|   All other functions are below   |
|----------------------------------*/

/*
    The displayMenu function will be used to display the menu options, VOID RETURN TYPE.
*/
void displayMenu(int* ptrOption)
{
    // Declare menu options:
    char menuOptions[3][24] = {
        "1. Play The Trivia Game",
        "2. View Scores",
        "3. Exit"
    };

    // Add space before menu:
    printf("\n");

    // Display the menu options:
    for (int i = 0; i < 3; i++){
        printf("\t%s\n", menuOptions[i]);
    }

    // Get user option choice:
    printf("\n\tEnter an option number: ");
    scanf("%d", ptrOption);
    printf("\n");
}

/*
    The getMenuOption will perform the action based on the user choice, VOID RETURN TYPE.
*/
void getMenuOption(int* ptrRunning, int* ptrOption, int score)
{

    // Determine action for chosen option:
    while(1){
        if (*ptrOption == 1){
            *ptrRunning = 1;
            break;
        }
        else if (*ptrOption == 2){
            viewScores();
            break;
        }
        else if (*ptrOption == 3){
            printf("\tGoodbye!\n\n");
            *ptrRunning = 0;
            break;
        }
        else{
            printf("\tInvalid Choice! Enter an option from 1 to 3: ");
            scanf("%d", ptrOption);
            printf("\n");
        }
    }
}

/*
    The randomNumberGen function will return a random number from 1 - 10, used to determine a random question, INT RETURN TYPE.
*/
int randomNumberGen()
{
    /*
        This is a random number generator. The function will return an int number 1 - 10.
    */

    return rand() % QUESTIONS + 1;
}

/*
    The getDataFromCSV function will gather the id, question, and answer for a record in the CSV called "questions.csv", VOID RETURN TYPE.
*/
void getDataFromCSV(CSVData* dataHolder, int randomNumber)
{
    /*
        This function will gather a random question from the CSV file holding all questions
        and answers
    */

    int id;
    char question[150];
    char answer[50];

    FILE* file;

    file = fopen("questions.csv", "r");

    if (file == NULL)
    {
        exit(1);
    }

    int read = 0;

    do
    {
        read = fscanf(file,"%d,%149[^,],%49[^,],", &id, question, answer);

        answer[strlen(answer)] = '\0';

        if (id == randomNumber)
        {
            dataHolder -> id = id;
            strcpy(dataHolder -> question, question);
            strcpy(dataHolder -> answer, answer);
            break;
        }
        
        
        if (read != 3 && !feof(file))
        {
            printf("File format incorrect.\n");
            exit(1);
        }

        if (ferror(file))
        {
            printf("Error reading file.\n");
            exit(1);
        }

    } while (!feof(file));

    fclose(file);
}

/*
    The askQuestion function will print the chosen random question to the screen, VOID RETURN TYPE.
*/
void askQuestion(CSVData* dataHolder)
{
    /*
        This function will print out a random question based off given argument value.
    */

    printf("\n\t%s: ", dataHolder -> question);
}

/*
    The grade function will take in the user's input as their answer, then see if the answer from the CSV are the same or different. If they are the same the user's score will be incremented by 1, else incorrect is printed, VOID RETURN TYPE.
*/
void grade(CSVData* dataHolder, int* userScore)
{
    /* 
        This function will determine if the answer was correct or incorrect. Then it will increment or deincrement the users score based off right or wrong.
    */

    // Variables
    char* userAnswer;
    userAnswer = (char*) malloc(sizeof(char) * 30);

    // Get the users answer and store it in userInputArray:
    scanf("%s", userAnswer);
    printf("\n");


    // Check to see if the user got it corrrect, if so increment their score:
    if (!strcmp(userAnswer, dataHolder -> answer)){
        printf("\tCorrect!\n");
        (*userScore)++;
    }

    // Else say incorrect:
    else{
        printf("\tIncorrect!\n");
    }

    // Free the memory of userInputArray:
    free(userAnswer);
}

/*
    The playAgain function will ask the user if they want to play again, if they type "yes" then they will be prompted with another question, else they will return to the menu, INT RETURN TYPE.
*/
int playAgain()
{
    // Variables
    int size = 10;

    // Allocate memory for yesOrNo char array:
    char* yesOrNo = (char*) malloc((size + 1) * sizeof(char));

    // Prompt user to play again and store it in the yesOrNo array:
    printf("\n\tDo you want to continue playing (Enter 'yes' or 'no'): ");
    scanf("%s", yesOrNo);
    printf("\n");

    // Check to see if they said yes or no:
    if (!strcmp(yesOrNo, "yes")){
        return 1;
    }
    else{
        return 0;
    }

    // Free the memory of yesOrNo:
    free(yesOrNo);
}

/*
    The endGame function is used to end the game, and print out the user's score, VOID RETURN TYPE.
*/
void endGame(int* ptrRunning, int* ptrScore)
{
    // This will run from 5 seconds till 1
    for (int t = 5; t > 0; t--){
        system("clear");
        printf("\n\tYou got %d questions correct!\n\n\tReturning to main menu in %d seconds...\n\n", *ptrScore, t);
        sleep(1);
        system("clear");
    }

    // Write data to the CSV
    writeData(ptrScore);

    // Tell while loop in main to quit the game
    *ptrRunning = 0;
}

/*
    The writeData function will write the user's score to another CSV file called "previous-scores.csv". This will act as a database, holding the users previous scores, and updating it to append new scores. VOID RETURN TYPE.
*/
void writeData(int* ptrScore)
{
    // Create a file pointer
    FILE* file;

    // Open the "previous-scores.csv" file in append mode so if the user doesnt have it then it will be created
    file = fopen("previous-scores.csv", "a");

    // Check if the file was able to be created or opened, exit if NULL, else write the score to the file
    if (file == NULL)
    {
        perror("Error locating or creating the file");
        exit(1);
    }

    // Create time struct to hold the current time
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    
    // Assign month, day, and year
    int month = local -> tm_mon + 1;
    int day = local -> tm_mday;
    int year = local -> tm_year + 1900;
    
    // Write the date to the file
    fprintf(file, "%02d/%02d/%04d, ", month, day, year);
    fprintf(file, "%d,\n", *ptrScore);

    // Close the file
    fclose(file);
}

/*
    The viewScores function will read data from the CSV "previous-scores.csv" and display them on the screen for the user to view, VOID RETURN TYPE.
*/
void viewScores()
{
    // Clear window to view the quesiton
    system("clear");
    printf("\n");

    // Create file pointer
    FILE* file;

    // Open the file
    file = fopen("previous-scores.csv", "r");

    // Check if file found
    if (file == NULL)
    {
        perror("Error locating the file");
        exit(1);
    }

    // Print out the headers
    printf("\tDate,\t\tScore\n");

    // Get the next lines of data and print them out
    int month = 0;
    int day = 0;
    int year = 0;
    int scoreFromCSV = 0;

    while(!feof(file))
    {
        // Get the date from the CSV
        fscanf(file, "%d/%d/%d, %d,\n", &month, &day, &year, &scoreFromCSV);

        // Print the data scanned in
        printf("\t%d/%d/%d\t%d\n", month, day, year, scoreFromCSV);        
    }

    // Close the file
    fclose(file);

    // Prompt user for input
    char userInput;
    int pressed = 0;

    while(!pressed)
    {
        printf("\n\tEnter 'c' or 'C' to continue: ");
        scanf("%s", &userInput);

        if (userInput == 'c' || userInput == 'C')
        {
            break;
        }
    }
}


