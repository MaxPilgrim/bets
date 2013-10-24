#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <string>
#include <dirent.h>
#include <string.h>
#include <cstdlib>
#include <map>
#include <cmath>
#include <malloc.h>
#include <stdlib.h>

using namespace std;
static int global_counter = 0;
class Error
{
public:
    static void error(const char *msg)
    {
        cout << "Error: " << msg << endl;
    }
    static void error(string msg)
    {
        cout << "Error: " << msg << endl;
    }
};

class Result
{
public:
    Result(void)
    {
        v[0][0] = -1; v[0][1] = -1; v[1][0] = -1; v[1][1] = -1;
    }
    Result(int x11, int x12, int x21, int x22)
    {
        v[0][0] = x11; v[0][1] = x12; v[1][0] = x21; v[1][1] = x22;
    }
    Result(int arr[2][2])
    {
        v[0][0] = arr[0][0]; v[0][1] = arr[0][1]; v[1][0] = arr[1][0]; v[1][1] = arr[1][1];
    }
    bool operator ==(Result &res) const
    {
        return ((v[0][0] == res.v[0][0]) && (v[0][1] == res.v[0][1]) && (v[1][0] == res.v[1][0]) && (v[1][1] == res.v[1][1]));
    }
    Result operator =(Result &res)
    {
        if ((v[0][0] != -1) || (v[0][1] != -1) || (v[1][0] != -1) || (v[1][1] != -1))
        {
            Error::error("result was changed although has already been set up");
        }
        v[0][0] = res.v[0][0];
        v[0][1] = res.v[0][1];
        v[1][0] = res.v[1][0];
        v[1][1] = res.v[1][1];
    }
    int v[2][2];
private:
    void transform(void);
};

class Date
{
public:
    Date (int d, int m, int y) : d(d), m(m), y(y) {}
    Date() : d(0), m(0), y(0) {}
    bool operator <(Date d)
    {
        if (y < d.y) { return true; } else if (y > d.y) { return false; }
        if (m < d.m) { return true; } else if (m > d.m) { return false; }
        if (this->d < d.d) { return true; } else if (this->d > d.d) { return false; }
        return false;
    }
    bool operator ==(Date d)
    {
        return ((y == d.y) && (m == d.m) && (this->d == d.d));
    }
    bool operator <=(Date d) { return operator <(d) || operator ==(d); }
    bool operator >(Date d) { return !operator<=(d); }
    bool operator >=(Date d) { return !operator<(d); }
    int days() const
    {
        int month[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
        return (y - 2013) * 365 + month[m-1] + (d-1);
    }
    static Date toDate(int x)
    {

        int month[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
        int year = x / 365;
        x -= year * 365;
        int mon = -1;
        for (int i = 0; i < 12; i++)
        {
            if (month[i] > x) { mon = i - 1; break; }
        }
        if (mon == -1) { mon = 11; }
        x -= month[mon];
        return Date(x+1, mon + 1, 2013 + year);
    }
    int operator +(Date date) const
    {
        return date.days() + days();
    }
    int operator -(Date date) const
    {
        int d1 = days();
        int d2 = date.days();
        if (d1 < d2) { return -1; }
        return d1 - d2;
    }
    static Date toDate(const char *s)
    {
        int y = (s[0] - 48) * 1000 + (s[1] - 48) * 100 + (s[2] - 48) * 10 + (s[3] - 48);
        int m = (s[5] - 48) * 10 + (s[6] - 48);
        int d = (s[8] - 48) * 10 + (s[9] - 48);
        return Date(d, m, y);
    }
    int d, m, y;
};
class Time
{
public:
    Time (const char *time) : h(0), m(0), s(0) {}
    Time (int h, int m, int s) : h(h), m(m), s(s) {}
    Time (int h, int m) : h(h), m(m), s(0) {}
    Time (void) : h(0), m(0), s(0) {}
    int seconds(void) const
    {
        return 3600 * h + 60 * m + s;
    }
    inline static Time toTime(int sec)
    {
        int x = sec - (sec / (24 * 3600)) * 24 * 3600;
        return Time(x / 3600, (x % 3600) / 60, x % 60);
    }
    inline bool operator <(Time t) const
    {
        return seconds() < t.seconds();
    }
    bool operator ==(Time t) { return seconds() == t.seconds(); }
    bool operator <=(Time t) { return operator <(t) || operator ==(t); }
    bool operator >(Time t) { return ! operator <=(t); }
    bool operator >=(Time t) { return ! operator <(t); }
    Time operator +(Time t)
    {
        return Time::toTime(t.seconds() + seconds());
    }
    Time operator -(Time t)
    {
        int t1 = seconds();
        int t2 = t.seconds();
        if (t1 < t2) { t1 += 24 * 3600; }
        int res = t1 - t2;
        return Time::toTime(res);
    }
    static Time transToTime(const char *s)
    {
        int h = (s[0] - 48) * 10 + (s[1] - 48);
        int m = (s[3] - 48) * 10  + (s[4] - 48);
        int sec = (s[6] - 48) * 10 + (s[7] - 48);
        return Time(h, m, sec);
    }
    int h, m, s;
private:
    void transform(void);
};
class Commands
{
public:
    Commands(const char *first, const char *second) : first(string(first)), second(string(second)){}
    Commands(void) : first(string("No_name")), second(string("No_name")) {}
    bool operator ==(Commands &cmds) const
    {
        return ((first == cmds.first) && (second == cmds.second));
    }
    string first;
    string second;
};
class StaticInfo
{
public:
    StaticInfo(Date &d, Time &t, Commands &c, Result &r) : resdate(d), restime(t), cmds(c), res(r) {}
    StaticInfo(void) : resdate(Date()), restime(Time()), cmds(Commands()), res(Result()) {}
    void init(Date &d, Time &t, Commands &c, Result &r)
    {
        resdate = d;
        restime = t;
        cmds = c;
        res = r;
    }
    Date resdate;
    Time restime;
    Commands cmds;
    Result res;
};
//typedef pair<Date, Time> Moment;
class Moment
{
public:
    Moment(void) : date(Date()), time(Time()) {}
    Moment(Date d, Time t) : date(d), time(t) {}
    bool operator <(Moment m)
    {
        if (date < m.date) { return true; }
        else if (date > m.date) { return false; }
        else
        {
            if (time < m.time) { return true; }
            else { return false; }
        }
    }
    bool operator ==(Moment m) { return (date == m.date) && (time == m.time); }
    bool operator <= (Moment m) { return operator <(m) || operator ==(m); }
    bool operator >(Moment m) { return ! operator <=(m); }
    bool operator >=(Moment m) { return ! operator <(m); }
    double operator -(Moment moment)  //in hours
    {
        if ((*this) < moment) { return -1; }
        int days = 0;
        if (time < moment.time)
        {
            days = date.toDate(date - moment.date) - Date(2, 1, 2013);
        }
        else { days = date - moment.date; }
        double t = (double)(time - moment.time).seconds();
        return 24*((double)days) + t / (3600);
    }
    double hours(void)
    {
        return operator-(Moment(Date(1, 1, 2013), Time(0, 0, 0)));
    }
    static Moment toMoment(double hours)
    {
        int days = floor(hours / 24);
        Date dt = Date::toDate(days);
        hours -= days * 24;
        int sec = round(hours * 3600);
        Time tm = Time::toTime(sec);
        return Moment(dt, tm);
    }
    Date date;
    Time time;
};
class Line
{
public:
    Line(void) : date(Date()), time(Time()), coeff(10, 0), bonuses(4, 0) {}
    Line(Date &date, Time &time, vector<double> &coeff, vector<double> &bonuses)
        : date(date), time(time), coeff(coeff), bonuses(bonuses) {}
    Date date;
    Time time;
    vector<double> coeff;
    vector<double> bonuses;
};
class Match
{
public:
    Match(void) : stinf(StaticInfo()), lines(vector<Line>()) {}
    Match(Date date, Time time, Commands cmds) : stinf(StaticInfo()), lines(vector<Line>())
    {
        stinf.resdate = date;
        stinf.restime = time;
        stinf.cmds = cmds;
    }
    Match(const char *path)
    {
        string ln;
        ifstream file(path);
        if (file.is_open())
        {
            int d, m, y;
            readDate(file, d, m, y);
            int hours, minutes, seconds;
            readTime(file, hours, minutes);
            getline(file, ln);
            const char *com1 = strdup(ln.data());
            getline(file, ln);
            const char *com2 = strdup(ln.data());
            int arr[2][2];
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    getline(file, ln);
                    arr[i][j] = atoi(ln.data());
                }
            }
            Date date(d, m, y);
            Time time(hours, minutes);
            Commands cmds(com1, com2);
            Result result(arr);
            stinf.init(date, time, cmds, result);  //initialization of StaticInfo field
            while (1)
            {
                int isOk = readDate(file, d, m, y);
                if (!isOk) { break; }  //testing if end of file
                readTime(file, hours, minutes, seconds);
                vector<double> coeff(10, 0);
                for (int i = 0; i < 10; i++)
                {
                    getline(file, ln);
                    coeff[i] = atof(ln.data());
                }
                vector<double> bonuses(4, 0);
                for (int i = 0; i < 4; i++)
                {
                    getline(file, ln);
                    bonuses[i] = atof(ln.data());
                }
                Date date(d, m, y);
                Time time(hours, minutes, seconds);
                Line line(date, time, coeff, bonuses);
                lines.push_back(line);
            }
            file.close();
        }
    }
    int bet_won(int kind) const
    {
        Result res = stinf.res;
        vector <double> bonuses(lines[0].bonuses);  // предположили, что бонусы не изменяются на протяжении всей игры. Нужно проверить!
        if ((res.v[0][0] < 0) || (res.v[0][1] < 1)) { return -1; }
        if (kind == 0) { return res.v[0][0] > res.v[0][1]; }
        else if (kind == 1) { return res.v[0][0] == res.v[0][1]; }
        else if (kind == 2) { return res.v[0][0] < res.v[0][1]; }
        else if (kind == 3) { return res.v[0][0] >= res.v[0][1]; }
        else if (kind == 4) { return res.v[0][0] != res.v[0][1]; }
        else if (kind == 5) { return res.v[0][0] <= res.v[0][1]; }
        else if (kind == 6) { return (res.v[0][0] + bonuses[0]) == res.v[0][1]; }
        else if (kind == 7) { return res.v[0][0] == (res.v[0][1] - bonuses[1]); }
        else if (kind == 8) { return (res.v[0][0] + res.v[0][1]) < bonuses[2]; }
        else if (kind == 9) { return (res.v[0][0] + res.v[0][1]) > bonuses[3]; }
        else return -1;
    }
    vector <double> get_coeff(int kind) const  //возвращает все коэффиценты на матч типа номер kind; 0<=kind<=9
    {
        vector <double> v;
        for (int i = 0; i < lines.size(); i++)
        {
            v.push_back(lines[i].coeff[kind]);
        }
        return v;
    }
    double max(vector<double> &v) const
    {
        double max = -1000;
        for (int i = 0; i < v.size(); i++)
        {
            if (v[i] > max) { max = v[i]; }
        }
        return max;
    }
    double min(vector<double> &v) const
    {
        double min = 1000;
        for (int i = 0; i < v.size(); i++)
        {
            if (v[i] < min) { min = v[i]; }
        }
        return min;
    }
    double Mcoeff(int kind)
    {
        vector<double> v = get_coeff(kind);
        double sum = 0.0;
        for (int i = 0; i < v.size(); i++)
        {
            sum += v[i];
        }
        if (v.size() == 0) { return -1; }
        return sum / v.size();
    }
    bool bonuses_eq(void) const  //это нужно для анализа 2
    {
        double b0 = lines[0].bonuses[0];
        double b1 = lines[0].bonuses[1];
        double b2 = lines[0].bonuses[2];
        double b3 = lines[0].bonuses[3];
        for (int i = 1; i < lines.size(); i++)
        {
            if (!((lines[i].bonuses[0] == b0) && (lines[i].bonuses[1] == b1) && (lines[i].bonuses[2] == b2) && (lines[i].bonuses[3] == b3))) { return false; }
        }
        return true;
    }
    bool check(void) const
    {
        bool isOk = true;
        for (int i = 0; i < lines.size(); i++)
        {
            for (int j = 0; j < 10; j++)
            {
                if (lines[i].coeff[j] < 1)
                {
                    cout << "Error: lines[" << i << "].coeff[" << j << "] = " << lines[i].coeff[j] << endl;
                    isOk = false;
                }
            }
        }
        Result res = stinf.res;
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                if (res.v[i][j] < 0) { cout << "Error in res: v[" << i << "][" << j << "] = " << res.v[i][j] << endl; isOk = false; }
            }
        }
        Date d = stinf.resdate;
        if ((d.d < 0) || (d.d > 31)) { cout << "Error in resdate: day = " << d.d << endl; isOk = false; }
        if ((d.m < 0) || (d.m > 12)) { cout << "Error in resdate: month = " << d.m<< endl; isOk = false; }
        if (d.y < 2013) { cout << "Error in resdate: year = " << d.y << endl; isOk = false; }
        Time t = stinf.restime;
        if ((t.h < 0) || (t.h > 23)){ cout << "Error in restime: day = " << t.h << endl; isOk = false; }
        if ((t.m < 0) || (t.m > 59)) { cout << "Error in restime: month = " << t.m<< endl; isOk = false; }
        if ((t.s < 0) || (t.s > 59)) { cout << "Error in restime: year = " << t.s << endl; isOk = false; }
        Commands c = stinf.cmds;
        if (strlen(c.first.data()) < 4) { cout << "Error in cmds: cmds.first = '" << c.first << "'" << endl; isOk = false; }
        if (strlen(c.second.data()) < 4) { cout << "Error in cmds: cmds.seconds = '" << c.second << "'" << endl; isOk = false; }
        return isOk;
    }
    bool hasResult(void) const
    {
        return ((stinf.res.v[0][0] >= 0) && (stinf.res.v[0][1] >= 0));
    }
    bool operator ==(Match &m)
    {
        bool b1 = (stinf.resdate.d == m.stinf.resdate.d) && (stinf.resdate.m == m.stinf.resdate.m) && (stinf.resdate.y == m.stinf.resdate.y);
        bool b2 = (stinf.restime.h == m.stinf.restime.h) && (stinf.restime.m == m.stinf.restime.m);
        bool b3 = (stinf.cmds == m.stinf.cmds);
        return b1 && b2 && b3;
    }
    StaticInfo stinf;
    vector<Line> lines;
private:
    int readDate(ifstream &file, int &d, int &m, int &y) const
    {
        string ln;
        if (!getline(file, ln)) { return 0; }  //end of file
        d = atoi(ln.data());
        getline(file, ln);
        m = atoi(ln.data());
        getline(file, ln);
        y = atoi(ln.data());
        return 1;  //correct
    }
    int readTime(ifstream &file, int &hours, int &minutes) const
    {
        string ln;
        if (!getline(file, ln)) { return 0; }  //end of file
        hours = atoi(ln.data());
        getline(file, ln);
        minutes = atoi(ln.data());
        return 1;  //correct
    }
    int readTime(ifstream &file, int &hours, int &minutes, int &seconds)
    {
        string ln;
        if (!getline(file, ln)) { return 0; }  //end of file
        hours = atoi(ln.data());
        getline(file, ln);
        minutes = atoi(ln.data());
        getline(file, ln);
        seconds = atoi(ln.data());
        return 1;  //correct
    }
};
class Restruct
{
public:
    Restruct(const char *source, const char *dest) : source(strdup(source)), dest(strdup(dest)) {}
    void run(void)
    {
        vector <string> dates;
        DIR *dir;
        dirent *ent;
        dir = opendir(source);
        while ((ent=readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))
            {
                dates.push_back(string(ent->d_name));
            }
        }
        closedir(dir);
        for (int i = 0; i < dates.size(); i++)
        {
            vector <string> times;
            string cur_path = string(source) + "/" + dates[i];  //содержится текущая директория. Внутри даты
            dir = opendir(cur_path.data());
            while ((ent=readdir(dir)) != NULL)
            {
                if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..") && (strcmp(ent->d_name, "results")))
                {
                    times.push_back(string(ent->d_name));
                }
            }
            closedir(dir);
            for (int j = 0; j < times.size(); j++)
            {
                string new_path = cur_path + "/" + times[j] + "/marathonbet.com/";  //находимся в директории с 4-мя файлами
                int result = read_file(new_path, dates[i].data(), times[j].data());
                if (result != 0) { Error::error("unexpected error in reading 4 files"); }
                /*new_path = cur_path + "/results/marathonbet.com";
                result = read_result(new_path);
                if (result != 0) { Error::error("unexpectrd error in reading results"); }*/
            }
            string new_path = cur_path + "/results/marathonbet.com";
            if (i == 10)
            {
                int k = 0;
            }
            int result = read_result(new_path);
            //if (result != 0) { Error::error("unexpected error in reading results"); }
        }
        //cout << "Done." << endl;
        out_matches("Matches");
    }
    void out_matches(const char *dir)
    {
        //DIR *directory = opendir(dir);
        system((string("mkdir -p ") + dir).data());
        for (int i = 0; i < matches.size(); i++)
        {
            Match m = matches[i];
            string path = string(dir) + "/" + to_string(i);
            fstream out;
            out.open(path.data(), ios::out);
            out << m.stinf.resdate.d << endl;
            out << m.stinf.resdate.m << endl;
            out << m.stinf.resdate.y << endl;
            out << m.stinf.restime.h << endl;
            out << m.stinf.restime.m << endl;
            out << m.stinf.cmds.first << endl;
            out << m.stinf.cmds.second << endl;
            out << m.stinf.res.v[0][0] << endl;
            out << m.stinf.res.v[0][1] << endl;
            out << m.stinf.res.v[1][0] << endl;
            out << m.stinf.res.v[1][1] << endl;
            for (int j = 0; j < m.lines.size(); j++)
            {
                out << m.lines[j].date.d << endl;
                out << m.lines[j].date.m << endl;
                out << m.lines[j].date.y << endl;
                out << m.lines[j].time.h << endl;
                out << m.lines[j].time.m << endl;
                out << m.lines[j].time.s << endl;
                for (int k = 0; k < 10; k++)
                {
                    out << m.lines[j].coeff[k] << endl;
                }
                for (int k = 0; k < 4; k++)
                {
                    out << m.lines[j].bonuses[k] << endl;
                }
            }
        }
    }
    int read_result(string path)
    {
        DIR *dir;
        dirent *ent;
        dir = opendir(path.data());
        if (dir == NULL) { Error::error("cannot find directory results"); return -1; }
        string file_name;
        while ((ent=readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))
            {
                //dates.push_back(string(ent->d_name));
                file_name = string(ent->d_name);
                break;  //может находиться несколько файлов. Достаточно рассмотреть любой из них
            }
        }
        closedir(dir);
        string full_path = path + "/" + file_name;
        ifstream input(full_path.data());
        string line1;
        int cnt = 0;
        while (getline(input, line1))
        {
            cnt++;
            if (cnt == 369)
            {
                cnt = 0;
            }
            string line2, line3;
            if (!getline(input, line2)) { Error::error("not enough lines if result file"); }
            if (!getline(input, line3)) { Error::error("not enough lines if result file"); }
            if (line3 == "Матч отменен")
            {
                Error::error(full_path + ": game was cancelled"); continue;
            }
            Moment moment = extract_moment(line1, Date(1, 1, 2013));  //здесь указывается любая дата
            const char *s = line2.data();
            int i = 1, k = 0;
            char *p1 = (char *)malloc(strlen(s));
            char *p2 = (char *)malloc(strlen(s));
            int len = strlen(s);
            while (i < len)
            {
                if (s[i] == ' ')
                {
                    if (s[i-1] == ' ')
                    {
                        break;
                    }
                    else { p1[k] = s[i-1]; k++; }
                }
                else
                {
                    p1[k] = s[i-1]; k++;
                }
                i++;
            }
            p1[k] = 0;  //terminated string
            k = 0;
            while ((s[i] == ' ') || (s[i] == '-')) { i++; }
            i++;
            while (i < len)
            {
                if (i >= len) { break; }
                if (s[i] == ' ')
                {
                    if (s[i-1] == ' ')
                    {
                        break;
                    }
                    else { p2[k] = s[i-1]; k++; }
                }
                else { p2[k] = s[i-1]; k++; }
                i++;
            }
            p2[k] = 0;
            Commands cmds(p1, p2);
            s = line3.data();
            int v11, v12, v21, v22;
            extract_result(line3, v11, v12, v21, v22);
            if ((v11 < 0) || (v11 > 20)) { Error::error(full_path + "wrong extraction of primary results(v11)"); cout << v11 << " " << v12 << " " << v21 << " " << v22 << endl; cout << line3 << endl; continue; }
            if ((v12 < 0) || (v12 > 20)) { Error::error(full_path + "wrong extraction of primary results(v12)"); cout << v11 << " " << v12 << " " << v21 << " " << v22 << endl; cout << line3 << endl; continue; }
            if ((v21 < 0) || (v21 > 9)) { Error::error(full_path + ": wrong extraction of secondary results(v21): "); cout << v11 << " " << v12 << " " << v21 << " " << v22 << endl; }
            if ((v22 < 0) || (v22 > 9)) { Error::error(full_path + "wrong extraction of secondary results(v22): "); cout << v11 << " " << v12 << " " << v21 << " " << v22 << endl; }
            int arr[2][2];
            arr[0][0] = v11; arr[0][1] = v12; arr[1][0] = v21; arr[1][1] = v22;
            Result res(arr);
            Match m(moment.date, moment.time, cmds);
            bool match_found = false;
            for (int i = 0; i < matches.size(); i++)
            {
                if (m == matches[i])
                {
                    global_counter++;
                    matches[i].stinf.res = res;
                    match_found = true;
                    break;
                }
            }
            if (!match_found)
            {
                m.stinf.res = res;
                matches.push_back(m);
            }
        }
        return 0;
    }
    int read_file(string path, const char *cur_date, const char *cur_time)
    {
        vector<double> coeff, bonuses;
        vector<Moment> moments;
        vector<Commands> cmds;
        const char *dates_path = strdup((path + string("dates")).data());
        const char *commands_path = strdup((path + "commands").data());
        const char *bonuses_path = strdup((path + "bonuses").data());
        const char *coeff_path = strdup((path + "coeff").data());
        ifstream dates_file(dates_path);
        ifstream commands_file(commands_path);
        ifstream bonuses_file(bonuses_path);
        ifstream coeff_file(coeff_path);
        string line;
        Moment moment;
        Date c_date = Date::toDate(cur_date);
        while (getline(dates_file, line))
        {
            moment = extract_moment(line, c_date);
            moments.push_back(moment);
            //cout << moment.date.d << " " << moment.date.m << " " << moment.date.y << " " << moment.time.h << " " << moment.time.m << endl;
        }
        dates_file.close();
        while (getline(commands_file, line))
        {
            string line2;
            if (!getline(commands_file, line2)) { Error::error("Disproportion in commands file"); break; }
            cmds.push_back(Commands(line.data(), line2.data()));
        }
        commands_file.close();
        while (getline(bonuses_file, line))
        {
            /* убраются начальные пробелы, но не конечные */
            const char *s = line.data();
            int i = 0;
            while (s[i] == ' ') { i++; }
            double value;
            if (s[i] == ';') { value = 1000.0; }
            else { value = atof(s+i); }
            bonuses.push_back(value);
        }
        bonuses_file.close();
        while (getline(coeff_file, line))
        {
            const char *s = line.data();
            int i = 0;
            while (s[i] == ' ') { i++; }
            double value = atof(s+i);
            if (value < 0) { Error::error("wrong info in coeff file. Value cannot be less than zero."); }
            coeff.push_back(value);
        }
        coeff_file.close();
        int size = moments.size();
        if ((cmds.size() != size) || (bonuses.size() != 4 * size) || (coeff.size() != 10 * size)) { Error::error(path + ": dispropotion in file sizes"); return -1; }
        for (int i = 0; i < moments.size(); i++)
        {
            Match m(moments[i].date, moments[i].time, cmds[i]);
            bool match_found = false;
            /* инициализация coeff и bonuses для соответствующего i */
            vector<double> c(10);
            for (int k = i * 10; k < i * 10 + 10; k++)
            {
                c[k - i * 10] = coeff[k];
            }
            vector<double> b(4);
            for (int k = i * 4; k < i * 4 + 4; k++)
            {
                b[k - i * 4] = bonuses[k];
            }
            Date n_date = Date::toDate(cur_date);
            Time n_time = Time::transToTime(cur_time);
            Line l(n_date, n_time, c, b);
            //Line l(moments[i].date, moments[i].time, c, b);
            /* конец инициализации  */
            for (int j = 0; j < matches.size(); j++)
            {
                if (m == matches[j])
                {
                    matches[j].lines.push_back(l);
                    match_found = true;
                    break;
                }
            }
            if (!match_found)
            {
                m.lines.push_back(l);
                matches.push_back(m);
            }
        }
        return 0;
    }
    Moment extract_moment(string line, Date cur_date) const
    {
        const char *s = line.data();
        Date date;
        Time time;
        int number_of_words = 0;
        int i = 0;
        while (i < strlen(s))
        {
            while (s[i] == ' ') { i++; }
            if (s[i] == 0) { break; }
            while (s[i] != ' ') { i++; }
            number_of_words++;
        }
        if (number_of_words == 1)  //неполная дата
        {
            i = 0;
            while (s[i] == ' ') { i++; }
            int hours = (s[i] - 48) * 10 + (s[i+1] - 48);
            i += 3;  //пропустили двоеточие
            int min = (s[i] - 48) * 10 + (s[i+1] - 48);
            date = cur_date;
            time = Time(hours, min, 0);
        }
        else  //полная дата
        {
            i = 0;
            while (s[i] == ' ') { i++; }
            int day = (s[i] - 48) * 10 + (s[i+1] - 48);
            i += 2;
            while (s[i] == ' ') { i++; }
            char m[7];  //первые три буквы месяца
            //m[0] = s[i]; m[1] = s[i+1]; m[2] = s[i+2]; m[3] = 0;
            m[0] = s[i]; m[1] = s[i+1]; m[2] = s[i+2]; m[3] = s[i+3];
            m[4] = s[i+4]; m[5] = s[i+5]; m[6] = 0;
            int month = identify_month(m);
            i += 7;  //вел себя неадекватно
            while (s[i] == ' ') { i++; }
            int hour = (s[i] - 48) * 10 + (s[i+1] - 48);
            i += 3;
            int min = (s[i] - 48) * 10 + (s[i+1] - 48);
            date = Date(day, month, cur_date.y);  //очень опасный переход. В новый год будут проблемы
            time = Time(hour, min, 0);
        }
        return Moment(date, time);
    }
    vector <Match> matches;
    char *source;
    char *dest;
private:
    void extract_result(string line, int &v11, int &v12, int &v21, int &v22) const
    {
        const char *s = line.data();
        int i = 0;
        int v[4] = {-1, -1, -1, -1};
        v11 = -1; v12 = -1; v21 = -1; v22 = -1;
        int k = 0;
        while (s[i] != 0)
        {
            while (!is_digit(s[i])) { if (s[i] == 0) { break; } i++; }
            if (s[i] == 0) { break; }
            i = trans_to_int(s, i, v[k]);
            k++;
            if (s[i] == 0) { break; }
        }
        v11 = v[0]; v12 = v[1]; v21 = v[2]; v22 = v[3];
        return;
    }
    bool is_digit(char c) const { return (c >= '0') && (c <= '9'); }
    int trans_to_int(const char *s, int pos, int &value) const
    {
        int i = pos;
        value = 0;
        while (s[i] != 0)
        {
            if ((s[i] < '0') || (s[i] > '9')) { break; }
            value *= 10;
            value += (s[i] - 48);
            i++;
        }
        return i;
    }
    string to_string(int x)
    {
        char *s = (char *)malloc(20);
        itoa(x, s);
        return string(s);
    }
    void reverse(char s[])
    {
        int i, j;
        char c;

        for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
            c = s[i];
            s[i] = s[j];
            s[j] = c;
        }
    }
    void itoa(int n, char s[])
    {
        int i, sign;

        if ((sign = n) < 0)  /* записываем знак */
            n = -n;          /* делаем n положительным числом */
        i = 0;
        do {       /* генерируем цифры в обратном порядке */
            s[i++] = n % 10 + '0';   /* берем следующую цифру */
        } while ((n /= 10) > 0);     /* удаляем */
        if (sign < 0)
            s[i++] = '-';
        s[i] = '\0';
        reverse(s);
    }
    int identify_month(char s[6]) const
    {
        if ((s[0]==-47) && (s[1]==-113) && (s[2]==-48) && (s[3]==-67) && (s[4]==-48) && (s[5]==-78)) { return 1; }
        else if ((s[0]==-47) && (s[1]==-124) && (s[2]==-48) && (s[3]==-75) && (s[4]==-48) && (s[5]==-78)) { return 2; }
        else if ((s[0]==-48) && (s[1]==-68) && (s[2]==-48) && (s[3]==-80) && (s[4]==-47) && (s[5]==-128)) { return 3; }
        else if ((s[0]==-48) && (s[1]==-80) && (s[2]==-48) && (s[3]==-65) && (s[4]==-47) && (s[5]==-128)) { return 4; }
        else if ((s[0]==-48) && (s[1]==-68) && (s[2]==-48) && (s[3]==-80) && (s[4]==-48) && (s[5]==-71)) { return 5; }
        else if ((s[0]==-48) && (s[1]==-72) && (s[2]==-47) && (s[3]==-114) && (s[4]==-48) && (s[5]==-67)) { return 6; }
        else if ((s[0]==-48) && (s[1]==-72) && (s[2]==-47) && (s[3]==-114) && (s[4]==-48) && (s[5]==-69)) { return 7; }
        else if ((s[0]==-48) && (s[1]==-80) && (s[2]==-48) && (s[3]==-78) && (s[4]==-48) && (s[5]==-77)) { return 8; }
        else if ((s[0]==-47) && (s[1]==-127) && (s[2]==-48) && (s[3]==-75) && (s[4]==-48) && (s[5]==-67)) { return 9; }
        else if ((s[0]==-48) && (s[1]==-66) && (s[2]==-48) && (s[3]==-70) && (s[4]==-47) && (s[5]==-126)) { return 10; }
        else if ((s[0]==-48) && (s[1]==-67) && (s[2]==-48) && (s[3]==-66) && (s[4]==-47) && (s[5]==-113)) { return 11; }
        else if ((s[0]==-48) && (s[1]==-76) && (s[2]==-48) && (s[3]==-75) && (s[4]==-48) && (s[5]==-70)) { return 12; }
        else return 0;
    }
};
class Analis
{
public:
    Analis(void) : games(vector<Match>()) {}
    Analis(vector<Match > &games) : games(games) {}
    Analis(const char *path)
    {
        DIR *dir;
        dirent *ent;
        dir = opendir(path);
        while ((ent=readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))
            {
                games.push_back(Match((string("Matches/")+string(ent->d_name)).data()));
            }
        }
        closedir(dir);
    }
    void analis2(int kind) const  //анализирует коэффиценты типа kind
    {
        cout << "Вывод всех коэффицентов типа kind = " << kind << ":" << endl;
        double delta = -1000.0;
        for (int i = 0; i < games.size(); i++)
        {
            cout << "*********************************" << endl;
            vector <double> coeff = games[i].get_coeff(kind);
            cout << "Матч: " << games[i].stinf.resdate.d << "-" << games[i].stinf.resdate.m << "-" << games[i].stinf.resdate.y << " ";
            cout << games[i].stinf.restime.h << ":" << games[i].stinf.restime.m << endl;
            cout << games[i].stinf.cmds.first << "-" << games[i].stinf.cmds.second << endl;
            print(coeff);
            double min = games[i].min(coeff);
            double max = games[i].max(coeff);
            cout << "Максимумумом является коэффицент: " << max << endl;
            cout << "Минимумом является коэффицент: " << min << endl;
            if ((max - min) > delta) { delta = max - min; }
            //cout << "*********************************" << endl;
            cout << "" << endl;
        }
        cout << "" << endl;
        cout << "Наибольшая дельта (max - min): " << delta << endl;
        /*bool all_bonuses_eq = true;
        for (int i = 0; i < games.size(); i++)
        {
           // if (!games[i].bonuses_eq()) { all_bonuses_eq = false; }
           cout << games[i].bonuses_eq() << endl;
        }
        cout << "Для каждого матча бонусы являются постоянными: " << all_bonuses_eq << endl;*/
        cout << "" << endl;
    }
    void analis3(int kind) const  //анализирует бонусы
    {
        bool all_bonuses_eq = true;
        for (int i = 0; i < games.size(); i++)
        {
            if (!games[i].bonuses_eq())
            {
                all_bonuses_eq = false;
                for (int j = 0; j < games[i].lines.size(); j++)
                {
                    for (int k = 0; k < 4; k++) { cout.width(6); cout << games[i].lines[j].bonuses[k]; }
                    cout << endl;
                }
                cout << "*******************************" << endl;
            }
        }
    }
    void analis4(int kind, double t0, int accuracy) const  //выявление положительного матожидания, построенного в момент времени t0
    {
        map<double, double> P;
        map<double, double> E;
        map<double, bool> Pos;
        map<double, int> All;
        int all = 0;
        for (int i = 0; i < games.size(); i++)
        {
            if (!games[i].hasResult()) { continue; }
            Moment start(games[i].stinf.resdate, games[i].stinf.restime);
            double time_start = start.hours();
            double ts = time_start - t0;
            if (ts < 0) { break; }
            Moment moment = Moment::toMoment(ts);
            for (int j = 0; j < games[i].lines.size(); j++)
            {
                Line line = games[i].lines[j];
                Moment curMoment(line.date, line.time);
                double delta = (curMoment < moment) ? moment - curMoment : curMoment - moment;
                if (delta > 0.5) { continue; }
                if (line.coeff[kind] == 0) { continue; }
                double k = Analis::rnd(line.coeff[kind], accuracy);
                int dt = games[i].bet_won(kind);
                if (dt == -1)  //это означает, что информация не доступна (не известна)
                {
                    continue;
                }
                P[k] += games[i].bet_won(kind); All[k] += 1;
                all++;
            }
        }
        cout << "Вывод вероятностей всех коэффицентов с шагом E" << accuracy << " для kind = " << kind << ":" << endl;
        for (map<double, double>::iterator i = P.begin(); i != P.end(); i++)
        {
            double first = (*i).first;
            //cout << first << "->" << (*i).second << " ";
            P[first] /= All[first];
            //cout << P[first] << " " << All[first] << endl;
        }
        cout << "***************************************************" << endl;
        for (map<double, double>::iterator i = P.begin(); i != P.end(); i++)
        {
            double k = (*i).first;
            E[k] = k * (*i).second - 1;  //E[k] = p * k
            Pos[k] = E[k] > 0;
            const int indent = 10;
            cout.width(indent); cout << k;
            //cout << k << P[k] << All[k] << E[k] << Pos[k] << endl;
            cout.width(indent); cout << P[k];
            cout.width(indent); cout << All[k];
            cout.width(indent); cout << E[k];
            cout.width(indent); cout << Pos[k] << endl;
        }
        cout << "Количество матчей, по которым проводились расчеты: " << all << endl;
    }
    void analis5(void) const  //проверка на наличие бадяги в исходных данных
    {
        bool isOk = true;
        for (int i = 0; i < games.size(); i++)
        {
            cout << "Анализ матча номер " << i << endl;
            if (!games[i].check()) { isOk = false; }
        }
        if (isOk) { cout << "Проверка на бадягу успешно пройдена. Бадяги не выявлено" << endl; }
        else { cout << "Бадяга была обнаружена. Конец провеки на бадягу" << endl; }
    }
    vector<Match> games;
    map<double, int> Pk;
private:
    void print(vector<double> &v, int amount = 10) const
    {
        for (int i = 1; i < v.size()+1; i++)
        {
            if (i % amount == 0) { cout << endl; }
            cout.width(5);
            cout << v[i-1];
        }
        cout << endl;
    }
public:
    static double rnd(double x, int k)
    {
        double p = pow(10, k);
        return round(x * p) / p;
    }
};
int main(int argc, char **argv)
{
    Restruct r("data", "Matches");
    r.run();
    Analis anal("Matches");
    anal.analis4(1, 12.0, 2);
    cout << "Global: " << global_counter << endl;
    return 0;
}