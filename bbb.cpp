using std::string;
using namespace std;

extern double sum, sum2, median, a, b, c, d, meandeviation, result, temp, am;
using json = nlohmann::json;

std::vector<std::string> explode(std::string & s, char delim)
{
	std::vector<std::string> resultv;
	std::istringstream iss(s);

	for (std::string token; std::getline(iss, token, delim); )
	{
			resultv.push_back(std::move(token));
	}

	return resultv;
}

std::vector<int> closest(std::vector<double> e, double value, int nw) {
	std::vector<double> x;
	for (double xn : e) {
		if (value < xn) {
			double newx = xn - value;
			x.push_back(newx);
		}
		else {
			double newx = value - xn;
			x.push_back(newx);
		}
	}
	std::vector<int> pos;
	for (int cl = 0; cl < nw; cl++) {
		auto minx = std::min_element(x.begin(), x.end());
		int pos2 = find(x.begin(), x.end(), *minx) - x.begin();
		pos.push_back(pos2);
		x[pos2] = 999999999999999;
	}
	return pos;
}

int main()
{
	std::string address = "35WGYwD9jtrqXf1z7eQsUwvBUK1ZfJSQhP";
	std::time_t result2 = std::time(nullptr);
	std::stringstream curtime;
	curtime << result2;
	time_t rawtime;
	struct tm * timeinfo;
	int hour = 0, min = 0, sec = 0;
	rawtime = time(0);
	timeinfo = gmtime(&rawtime);
	timeinfo->tm_hour = hour;
	timeinfo->tm_min = min;
	timeinfo->tm_sec = sec;
	std::time_t daybeginning = _mkgmtime(timeinfo);
	std::ifstream orgaddr("org.txt", std::ifstream::in);
	std::string orgaddress;
	std::getline(orgaddr, orgaddress);
	system("bitcoin-cli listtransactions \"*\" 9999 0 > tmp.txt");
	std::ifstream fp("tmp.txt", std::ifstream::in);
	json p;
	fp >> p;
	std::ifstream db("db.txt", std::ifstream::in);
	std::string db2;
	std::getline(db, db2);
	std::vector<std::string> exp = explode(db2, ',');
	int y = exp.size();
	std::vector<std::string> olds;
	for (int z = 0; z < y; z++)
	{
		auto exp2 = explode(exp[z], '|');
		olds.push_back(exp2[0]);
	}
	int sz = p.size();
	int g = 0;
	for (int o = 0; o < sz; o++) {
		if (p[o]["category"] == "receive" && p[o]["confirmations"] >= 6 && p[o]["address"] == address && p[o]["time"] < daybeginning) {
			std::string txid = p[o]["txid"];
			stringstream stream;
			stream << fixed << setprecision(8);
			if (std::find(olds.begin(), olds.end(), txid) == olds.end())
			{
				system(("tx " + txid + " > txtmp.txt").c_str());
				std::ifstream txtmp("txtmp.txt", std::ifstream::in);
				json tt;
				txtmp >> tt;
				std::string addr = tt["inputs"][0]["prev_out"]["addr"];
				double am = p[o]["amount"];
				stream << am;
				std::string newgame = stream.str() + "|" + addr + "|" + txid + ",";
				std::string newold = txid + "|" + curtime.str() + ",";
				ofstream fileOUT("db.txt", ios::app);
				fileOUT << newold;
				fileOUT.close();
				ofstream fileOUT2(curtime.str() + ".txt", ios::app);
				fileOUT2 << newgame;
				fileOUT2.close();
				stream.str("");
				g++;
			}
		}
	}

	// initializing new game
	std::ifstream dbgame(curtime.str() + ".txt", std::ifstream::in);
	std::string dbgame2;
	std::getline(dbgame, dbgame2);
	std::vector<std::string> num;
	std::vector<std::string> parts = explode(dbgame2, ',');
	int ps = parts.size();

	// creating pool of numbers
	std::vector<double> e;
	std::vector<double> l;
	std::vector<std::string> addresses;
	std::vector<std::string> txids;
	for (int bet = 0; bet < ps; bet++)
	{
		num = explode(parts[bet], '|');
		e.push_back(::atof(num[0].c_str()));
		l.push_back(0);
		addresses.push_back(num[1]);
		txids.push_back(num[2]);
	}
	std::vector<double> e2(e);
	int winners = 0;

	// determining number of winners
	if (g == 0)	{
		std::cout << "No bets!";
		system("pause");
	}
	else if (g == 1) {
		stringstream stream;
		stream << fixed << setprecision(8);
		stream << num[0];
		system(("bitcoin-cli sendtoaddress \"" + num[1] + "\" " + stream.str() + " \"\" \"\" true").c_str());
		std::cout << "Only 1 bet! Refund!" << std::endl;
		system("pause");
	} else if (g > 1 && g <= 10) {
		winners = 1;
	}
	else if (g > 10 && g <= 30) {
		winners = 3;
	}
	else if (g > 30 && g <= 50) {
		winners = 5;
	}
	else if (g > 50) {
		winners = 10;
	}

	// calculating final result
	if (winners != 0) {
		double sum = 0;
		double sum2 = 0;
		for (int i = 0;i < g;i++)
		{
			sum = sum + e[i];
		}
		double a = log(sum * g);
		double b = sum / g;
		double c = log(b) / log(sum);

		for (int r = 0;r < g;r++)
		{
			if (e[r] < b) {
				l[r] = b - e[r];
			}
			else {
				l[r] = e[r] - b;
			}
			sum2 = sum2 + l[r];
		}
		double meandeviation = sum2 / g;

		for (int s = 0;s < g;s++)
		{
			for (int j = 0;j < g - 1 - s;j++)
			{
				if (e[j] > e[j + 1])
				{
					double temp = e[j];
					e[j] = e[j + 1];
					e[j + 1] = temp;
				}
			}
		}
		double median = 0;
		if (g % 2 == 0) {
			median = (e[g / 2 - 1] + e[g / 2]) / 2;
		}
		else {
			median = e[g / 2];
		}
		double d = (median * g) / b;

		double result = (c / d + 2 * log(b) - meandeviation) / a;

		if (result < 0) {
			result = result * -1;
		}

		//finding winners and sending payments
		if (winners == 1) {
			std::vector<int> posi = closest(e2, result, winners);
			double win1 = e2[posi[0]] + (sum - e2[posi[0]])*0.9;
			double org = sum - win1;
			stringstream stream;
			stream << fixed << setprecision(8);
			stream << win1;
			std::string sendwin1 = stream.str();
			system(("bitcoin-cli sendtoaddress \"" + addresses[posi[0]] + "\" " + sendwin1 + " \"\" \"\" true").c_str());
			stream.str("");
			stream << org;
			std::string sendorg = stream.str();
			system(("bitcoin-cli sendtoaddress \"" + orgaddress + "\" " + sendorg + " \"\" \"\" true").c_str());
			stream.str("");
			stream << result;
			std::cout << "Winning number: " << stream.str() << std::endl;
			std::cout << "Participants: " << g << std::endl;
			std::cout << "Total winners: " << winners << std::endl;
			stream.str("");
			stream << sum;
			std::cout << "Bitcoins played: " << stream.str() << std::endl;
			std::cout << "Winning address: " << addresses[posi[0]] << std::endl;
			std::cout << "Winning txid: " << txids[posi[0]] << std::endl;
			stream.str("");
			stream << e[posi[0]];
			std::cout << "Bet: " << stream.str() << std::endl;
			std::cout << "Payout: " << sendwin1 << std::endl;
			std::cout << "---" << std::endl;
			std::cout << "Dev reward: " << sendorg << std::endl;
			system("pause");
		}
		else if (winners == 3) {
			std::vector<int> posi = closest(e2, result, winners);
			double sumwin = sum - (e2[posi[0]] + e2[posi[1]] + e2[posi[2]]);
			std::vector<double> win;
			double win1 = e2[posi[0]] + sumwin*0.5;
			double win2 = e2[posi[1]] + sumwin*0.3;
			double win3 = e2[posi[2]] + sumwin*0.1;
			double org = sum - (win1 + win2 + win3);
			win.push_back(win1);
			win.push_back(win2);
			win.push_back(win3);
			stringstream stream;
			stream << fixed << setprecision(8);
			stream << result;
			std::cout << "Winning number: " << stream.str() << std::endl;
			std::cout << "Participants: " << g << std::endl;
			std::cout << "Total winners: " << winners << std::endl;
			stream.str("");
			stream << sum;
			std::cout << "Bitcoins played: " << stream.str() << std::endl;
			std::cout << "---" << std::endl;
			for (int ws = 0; ws < winners; ws++) {
				stream.str("");
				stream << win[ws];
				std::string sendwin = stream.str();
				system(("bitcoin-cli sendtoaddress \"" + addresses[posi[ws]] + "\" " + sendwin + " \"\" \"\" true").c_str());
				std::cout << "Place: " << ws + 1 << std::endl;
				std::cout << "Winning address: " << addresses[posi[ws]] << std::endl;
				std::cout << "Winning txid: " << txids[posi[ws]] << std::endl;
				stream.str("");
				stream << e2[posi[ws]];
				std::cout << "Bet: " << stream.str() << std::endl;
				std::cout << "Payout: " << sendwin << std::endl;
				std::cout << "---" << std::endl;
			}
			stream.str("");
			stream << org;
			std::string sendorg = stream.str();
			system(("bitcoin-cli sendtoaddress \"" + orgaddress + "\" " + sendorg + " \"\" \"\" true").c_str());
			std::cout << "Dev reward: " << sendorg << std::endl;
			system("pause");
		}
		else if (winners == 5) {
			std::vector<int> posi = closest(e2, result, winners);
			double sumwin = sum - (e2[posi[0]] + e2[posi[1]] + e2[posi[2]] + e2[posi[3]] + e2[posi[4]]);
			std::vector<double> win;
			double win1 = e2[posi[0]] + sumwin*0.3;
			double win2 = e2[posi[1]] + sumwin*0.22;
			double win3 = e2[posi[2]] + sumwin*0.16;
			double win4 = e2[posi[3]] + sumwin*0.12;
			double win5 = e2[posi[4]] + sumwin*0.1;
			double org = sum - (win1 + win2 + win3 + win4 + win5);
			win.push_back(win1);
			win.push_back(win2);
			win.push_back(win3);
			win.push_back(win4);
			win.push_back(win5);
			stringstream stream;
			stream << fixed << setprecision(8);
			stream << result;
			std::cout << "Winning number: " << stream.str() << std::endl;
			std::cout << "Participants: " << g << std::endl;
			std::cout << "Total winners: " << winners << std::endl;
			stream.str("");
			stream << sum;
			std::cout << "Bitcoins played: " << stream.str() << std::endl;
			std::cout << "---" << std::endl;
			for (int ws = 0; ws < winners; ws++) {
				stream.str("");
				stream << win[ws];
				std::string sendwin = stream.str();
				system(("bitcoin-cli sendtoaddress \"" + addresses[posi[ws]] + "\" " + sendwin + " \"\" \"\" true").c_str());
				std::cout << "Place: " << ws + 1 << std::endl;
				std::cout << "Winning address: " << addresses[posi[ws]] << std::endl;
				std::cout << "Winning txid: " << txids[posi[ws]] << std::endl;
				stream.str("");
				stream << e2[posi[ws]];
				std::cout << "Bet: " << stream.str() << std::endl;
				std::cout << "Payout: " << sendwin << std::endl;
				std::cout << "---" << std::endl;
			}
			stream.str("");
			stream << org;
			std::string sendorg = stream.str();
			system(("bitcoin-cli sendtoaddress \"" + orgaddress + "\" " + sendorg + " \"\" \"\" true").c_str());
			std::cout << "Dev reward: " << sendorg << std::endl;
			system("pause");
		}
		else if (winners == 10) {
			std::vector<int> posi = closest(e2, result, winners);
			double sumwin = sum - (e2[posi[0]] + e2[posi[1]] + e2[posi[2]] + e2[posi[3]] + e2[posi[4]] + e2[posi[5]] + e2[posi[6]] + e2[posi[7]] + e2[posi[8]] + e2[posi[9]]);
			std::vector<double> win;
			double win1 = e2[posi[0]] + sumwin*0.25;
			double win2 = e2[posi[1]] + sumwin*0.19;
			double win3 = e2[posi[2]] + sumwin*0.14;
			double win4 = e2[posi[3]] + sumwin*0.1;
			double win5 = e2[posi[4]] + sumwin*0.07;
			double win6 = e2[posi[5]] + sumwin*0.05;
			double win7 = e2[posi[6]] + sumwin*0.04;
			double win8 = e2[posi[7]] + sumwin*0.03;
			double win9 = e2[posi[8]] + sumwin*0.02;
			double win10 = e2[posi[9]] + sumwin*0.01;
			double org = sum - (win1 + win2 + win3 + win4 + win5 + win6 + win7 + win8 + win9 + win10);
			win.push_back(win1);
			win.push_back(win2);
			win.push_back(win3);
			win.push_back(win4);
			win.push_back(win5);
			win.push_back(win6);
			win.push_back(win7);
			win.push_back(win8);
			win.push_back(win9);
			win.push_back(win10);
			stringstream stream;
			stream << fixed << setprecision(8);
			stream << result;
			std::cout << "Winning number: " << stream.str() << std::endl;
			std::cout << "Participants: " << g << std::endl;
			std::cout << "Total winners: " << winners << std::endl;
			stream.str("");
			stream << sum;
			std::cout << "Bitcoins played: " << stream.str() << std::endl;
			std::cout << "---" << std::endl;
			for (int ws = 0; ws < winners; ws++) {
				stream.str("");
				stream << win[ws];
				std::string sendwin = stream.str();
				system(("bitcoin-cli sendtoaddress \"" + addresses[posi[ws]] + "\" " + sendwin + " \"\" \"\" true").c_str());
				std::cout << "Place: " << ws + 1 << std::endl;
				std::cout << "Winning address: " << addresses[posi[ws]] << std::endl;
				std::cout << "Winning txid: " << txids[posi[ws]] << std::endl;
				stream.str("");
				stream << e2[posi[ws]];
				std::cout << "Bet: " << stream.str() << std::endl;
				std::cout << "Payout: " << sendwin << std::endl;
				std::cout << "---" << std::endl;
			}
			stream.str("");
			stream << org;
			std::string sendorg = stream.str();
			system(("bitcoin-cli sendtoaddress \"" + orgaddress + "\" " + sendorg + " \"\" \"\" true").c_str());
			std::cout << "Dev reward: " << sendorg << std::endl;
			system("pause");
		}
	}
	return 0;
}