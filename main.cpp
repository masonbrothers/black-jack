#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

struct card {
	int value;
	int toll;
	string symbol;
};

struct roundInfo
{
	double money;
	bool insurance;
	bool doubleDown;
	string discription;
	vector<card> cardsPlayed;
};

int countTotalCards(const vector<card> &cards);

int countValueOfCards(const vector<card> &cards);

vector<card> setUpDeck();

vector<card> setUpDeck(const int &decks);

string printCards(const vector<card> &cards);

double guessNextCard(const vector<card> &cardsInDeck);

vector<card> subtractPiles(const vector<card> &pile1, const vector<card> &pile2);

vector<card> addPiles(const vector<card> &pile1, const vector<card> &pile2);

int countPoints(const vector<card> &hand);

string removeSpacing(string input);

vector<card> findCard(const string &theCard);

bool isBlackJack(const vector<card> &hand);

bool doubleDown(const vector<card> &myHand, const vector<card> &dealer, const vector<card>& cards);

double chanceOf(const string &symbol, const vector<card> &cards);

bool getInsurance(vector<card> dealer, const vector<card> &cards, double minChanceBlackJack);

string getChances(const vector<card> &cards);

double getBalance(const vector<roundInfo> &rounds);

roundInfo playARound(const vector<card> &cards);

int main()
{

	vector<card> cards = setUpDeck(8);
	vector<roundInfo> rounds;

	for (int i = 0; 1; i++)
	{

		rounds.push_back(playARound(cards));
		roundInfo tempRound = rounds.at(i);
		cout << "Your balance changed by: " << tempRound.money << endl;
		cout << "You are up: " << getBalance(rounds) << endl;
		cout << endl;
		cards = subtractPiles(cards,rounds.at(i).cardsPlayed);
		if (i>=1)
		{
			cards = addPiles(cards,rounds.at(i-1).cardsPlayed);
		}

		//Add Cuts to deck and stuff

	}
	/*
	vector<card> remaining = subtractPiles(cards,inPlay);

	cout << printCards(remaining);
    */
    return 0;
}

double getBalance(const vector<roundInfo> &rounds)
{
	double balance = 0;
	for (int i = 0; i < rounds.size(); i++)
	{
		balance += rounds.at(i).money;
	}
	return balance;
}

roundInfo playARound(const vector<card> &originalCards)
{
	vector<card> otherPlayers = setUpDeck(0);
	string temp;
	roundInfo thisRound;
	double OriginalWager = 0;
	double wager = OriginalWager;
	double insurance = 0;
	vector<card> cards = originalCards;
	cout << "How much are you betting?" << endl;
	cin >> OriginalWager;

	wager = OriginalWager;
	cout << "What cards are on the table (not your hand or dealer)? End with \"!\"." << endl;
	while (temp!="!")
	{
		cin >> temp;
		otherPlayers = addPiles(otherPlayers,findCard(temp));
	}

	//Dealer
	cout << "What does the dealer have face up?" << endl;
	cin >> temp;
	vector<card> dealer = setUpDeck(0);
	dealer = findCard(temp);

	thisRound.insurance = 0;
	if (getInsurance(dealer,cards,0))
	{
		cout << "You might want to get insurance. Get it (Y)?";
		cin >> temp;
		if (temp == "Y")
		{
			insurance = (double)OriginalWager/2;
			thisRound.insurance = 1;
		}
		else
		{
			thisRound.insurance = 0;
		}
	}

	//You
	cout << "What is your hand? End with \"!\"." << endl;
	vector<card> myHand = setUpDeck(0);
	cout << "Card 1: ";
	cin >> temp;
	myHand = addPiles(myHand,findCard(temp));
	cout << "Card 2: ";
	cin >> temp;
	myHand = addPiles(myHand,findCard(temp));

	cout << "What did those before you get when they hit?  End with \"!\"." << endl;
	while (temp!="!")
	{
		cin >> temp;
		otherPlayers = addPiles(otherPlayers,findCard(temp));
	}

	vector<card> inPlay = setUpDeck(0);
	inPlay = addPiles(inPlay,otherPlayers);
	inPlay = addPiles(inPlay,dealer);
	inPlay = addPiles(inPlay,myHand);
	cards = subtractPiles(cards,inPlay);

	//FIGURE OUT WHAT TO DO

	thisRound.doubleDown = 0;
	if (doubleDown(myHand,dealer,cards))
	{
		cout << "You might want to double down. Double down (Y)?";
		cin >> temp;
		if (temp == "Y")
		{
			wager*=2;
			thisRound.doubleDown = 1;
		}
		else
		{
			thisRound.doubleDown = 0;
		}
	}


	while(!thisRound.doubleDown)
	{
		if ((countPoints(myHand)+guessNextCard(cards))<=21)
		{
			cout << "Hit!" << "\t(" << countPoints(myHand) << ", " << guessNextCard(cards) << ")" << endl;
			cin >> temp;
			myHand = addPiles(myHand,findCard(temp));
		}
		else
		{
			cout << "Stay!" << "\t(" << countPoints(myHand) << ", " << guessNextCard(cards) << ")" << endl;
			break;
		}
	}


	//cout << printCards(cards);



	cout << "What did those after you get when they hit (not the dealer)?  End with \"!\"." << endl;
	while (temp!="!")
	{
		cin >> temp;
		inPlay = addPiles(inPlay,findCard(temp));
	}
	cout << "What was the dealers hidden card?" << endl;
	cin >> temp;
	inPlay = addPiles(inPlay,findCard(temp));
	dealer = addPiles(dealer,findCard(temp));

	cout << "What did the dealer get when he/she hit? End with \"!\"." << endl;
	while (temp!="!")
	{
		cin >> temp;
		inPlay = addPiles(inPlay,findCard(temp));
		dealer = addPiles(dealer,findCard(temp));
	}

	cout << "I have " << countPoints(myHand) << " points." << endl;
	cout << "The dealer has " << countPoints(dealer) << " points." << endl;

	if ((countPoints(myHand)<=21 && (countPoints(myHand) > countPoints(dealer))) || (isBlackJack(myHand) && !isBlackJack(dealer)))
	{
		cout << "I win!" << endl;

		if (isBlackJack(myHand))
		{
			wager=(double)wager*3/2;
			thisRound.discription = "Win - BlackJack";
		}
		else
		{
			thisRound.discription = "Win - Regular";
			wager *= 2;
		}
	}
	if (countPoints(myHand)>21 || (countPoints(myHand) < countPoints(dealer)) || (isBlackJack(dealer) && !isBlackJack(myHand)))
	{
		cout << "Dealer wins. :(" << endl;
		thisRound.discription = "Loss";
		wager = 0;
	}


	//TIE & BlackJack

	if ((countPoints(myHand) == countPoints(dealer) && !(isBlackJack(myHand)||isBlackJack(dealer))))
	{
		cout << "Push" << endl;
		thisRound.discription = "Push";
	}
	if (isBlackJack(myHand) && isBlackJack(dealer))
	{
		cout << "Push" << endl;
		thisRound.discription = "Push";
	}


	if (thisRound.doubleDown == 1)
	{
		thisRound.discription.append(" (Double Down)");
	}
	if (isBlackJack(dealer) && thisRound.insurance == 1)
	{
		insurance*=2;
		cout << "Won Insurance!" << endl;
		thisRound.discription.append(" - Won Insurance");
	}
	else
	{
		insurance=0;
		cout << "Lost Insurance!" << endl;
		thisRound.discription.append(" - Lost Insurance");
	}
	thisRound.cardsPlayed = inPlay;
	thisRound.money = wager + insurance - OriginalWager;
	return thisRound;
}

string getStatistics(const vector<card> &cards)
{
	ostringstream output;
	output << "Total Number of Cards: " << countTotalCards(cards) << endl;
	output << "Total Value of Cards: " << countValueOfCards(cards) << endl;
	output << "The next card is probably: " << guessNextCard(cards) << endl;
	output << "Cards in Deck: ";
	output << printCards(cards) << endl;
	output << getChances(cards);
	return output.str();
}

string getChances(const vector<card> &cards)
{
	ostringstream output;
	output << "Chances of Value:" << endl;
	output << "1/11:\t" << chanceOf("A",cards) << endl;
	output << "2:\t" << chanceOf("2",cards) << endl;
	output << "3:\t" << chanceOf("3",cards) << endl;
	output << "4:\t" << chanceOf("4",cards) << endl;
	output << "5:\t" << chanceOf("5",cards) << endl;
	output << "6:\t" << chanceOf("6",cards) << endl;
	output << "7:\t" << chanceOf("7",cards) << endl;
	output << "8:\t" << chanceOf("8",cards) << endl;
	output << "9:\t" << chanceOf("9",cards) << endl;
	double ten = 0;
	ten += chanceOf("10",cards);
	ten += chanceOf("J",cards);
	ten += chanceOf("Q",cards);
	ten += chanceOf("K",cards);
	output << "10:\t" << ten << endl;
}

bool doubleDown(const vector<card> &myHand, const vector<card> &dealer, const vector<card>& cards)
{
	double dealerCount =0;
	double myCount = 0;
	myCount = countPoints(myHand)+guessNextCard(cards);
	dealerCount = countPoints(dealer);
	while (dealerCount < 17)
	{
		dealerCount +=guessNextCard(cards);
	}
	if (dealerCount<myCount)
	{
		return 1;
	}
	return 0;
}

double chanceOf(const string &symbol, const vector<card> &cards)
{
	//findCard(symbol);
	int toll;
	for (int i=0; i < cards.size(); i++)
	{
		if (symbol == cards.at(i).symbol)
		{
			toll = cards.at(i).toll;
			break;
		}
	}
	return (double)toll/countTotalCards(cards);
}

//roundInfo playARound

bool getInsurance(vector<card> dealer, const vector<card> &cards, double minChanceBlackJack)
{
	if (!(dealer.at(0).toll==1))
	{
		return 0;
	}
	double chance = 0;
	chance += chanceOf("10", cards);
	chance += chanceOf("J", cards);
	chance += chanceOf("Q", cards);
	chance += chanceOf("K", cards);
	if (minChanceBlackJack < chance)
	{
		return 1;
	}
	return 0;
}

string action()
{
	/*
	"Insurance";//only on ace
	//"Fold";
	"Hit";
	"Stay";
	"Double Down";
	"Split";
	*/
}

bool isBlackJack(const vector<card> &hand)
{
	if (countTotalCards(hand)!=2)
	{
		return 0;
	}
	if (hand.at(0).toll==1)
	{
		for (int i = 9; i < hand.size(); i++)
		{
			if (hand.at(i).toll==1)
			{
				return 1;
			}
		}
	}
	return 0;
}

bool canSplit(const vector<card> &hand, bool doubleSplit)
{
	if(countTotalCards(hand)!=2 && countTotalCards(hand)!=4)
	{
		return 0;
	}
	if (doubleSplit==0 || countTotalCards(hand)==4)
	{
		return 0;
	}
	int value;
	for (int i = 0; i < hand.size(); i++)
	{
		if (hand.at(i).toll >=1)
		{
			value = hand.at(i).value;
		}
	}
	if (value == (double)countValueOfCards(hand)/countTotalCards(hand))
	{
		return 1;
	}
	return 0;
}


string removeSpacing(string input)
{
	for (int i = 0; i < input.size(); i++)
	{
		if (input.at(i)=='\t' || input.at(i)=='\n' || input.at(i)=='\r' || input.at(i)==' ')
		{
			input.erase(i);
		}
	}
	return input;
}

vector<card> findCard(const string &theCard)
{
	vector<card> cards = setUpDeck(0);
	for (int i = 0; i < cards.size(); i++)
	{
		if (cards.at(i).symbol == removeSpacing(theCard))
		{
			cards.at(i).toll = 1;
			return cards;
		}
	}
	return cards;
}

vector<card> subtractPiles(const vector<card> &pile1, const vector<card> &pile2)
{
	vector<card> cards = setUpDeck();
	if (pile1.size()!=pile2.size() || pile1.size()!=cards.size())
	{
		for (int i = 0; i < cards.size(); i++)
		{
			cards.at(i).toll = 0;
		}
		return cards;
	}
	for (int i = 0; i < cards.size(); i++)
	{
		cards.at(i).toll = pile1.at(i).toll - pile2.at(i).toll;
	}
	return cards;
}

vector<card> addPiles(const vector<card> &pile1, const vector<card> &pile2)
{
	vector<card> cards = setUpDeck();
	if (pile1.size()!=pile2.size() || pile1.size()!=cards.size())
	{
		for (int i = 0; i < cards.size(); i++)
		{
			cards.at(i).toll = 0;
		}
		return cards;
	}
	for (int i = 0; i < cards.size(); i++)
	{
		cards.at(i).toll = pile1.at(i).toll + pile2.at(i).toll;
	}
	return cards;
}

double guessNextCard(const vector<card> &cardsInDeck)
{
	return (double)countValueOfCards(cardsInDeck)/countTotalCards(cardsInDeck);
}

string printCards(const vector<card> &cards)
{
	ostringstream cardCount;
	for (int i = 0; i < cards.size(); i++)
	{
		cardCount << cards.at(i).symbol << ":\t" << cards.at(i).toll << endl;
	}
	return cardCount.str();
}

int countTotalCards(const vector<card> &cards)
{
	int toll = 0;
	for (int i = 0; i < cards.size(); i++)
	{
		 toll += cards.at(i).toll;
	}
	return toll;
}

int countValueOfCards(const vector<card> &cards)
{
	int values =0;
	for (int i = 0; i < cards.size(); i++)
	{
		values += cards.at(i).value*cards.at(i).toll;
	}
	return values;
}

int countPoints(const vector<card> &hand)
{
	int points = 0;
	for (int i = 1; i < hand.size(); i++)
	{
		points += hand.at(i).toll*hand.at(i).value;
	}
	for (int i = 0; i < hand.at(0).toll; i++)
	{
		points += 1;
	}
	if (hand.at(0).toll > 0)
	{
		//cout << hand.at(0).toll << "\t" << points << endl;

		if ((points + 10) > 21)
		{
			//cout << "@" << hand.at(0).toll << "\t" << points << endl;
			return points;
		}
		else
		{
			points += 10;
			//cout << "@" << hand.at(0).toll << "\t" << points << endl;
			return points;
		}
	}
	return points;
}

vector<card> setUpDeck()
{
	return setUpDeck(1);
}
vector<card> setUpDeck(const int &decks)
{
	vector<card> cards (13);
	cards.at(0).value = 1;//11 or 6
	cards.at(0).symbol = "A";
	for (int i = 1; i < 10; i++)
	{
		cards.at(i).value = i+1;
		ostringstream temp;
		temp << (i+1);
		cards.at(i).symbol = temp.str();
	}
	for (int i = 10; i < cards.size(); i++)
	{
		cards.at(i).value = 10;
	}
	cards.at(10).symbol = "J";
	cards.at(11).symbol = "Q";
	cards.at(12).symbol = "K";
	for (int i = 0; i < cards.size(); i++)
	{
		cards.at(i).toll = 4*decks;
	}
	return cards;
}


