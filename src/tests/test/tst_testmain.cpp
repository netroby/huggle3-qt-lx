//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#include <QString>
#include <iostream>
#include <QtTest>
#include <huggle_core/huggleparser.hpp>
#include <huggle_core/configuration.hpp>
#include <huggle_core/generic.hpp>
#include <huggle_core/wikiedit.hpp>
#include <huggle_core/wikipage.hpp>
#include <huggle_core/wikisite.hpp>
#include <huggle_core/sleeper.hpp>
#include <huggle_core/terminalparser.hpp>
#include <huggle_core/wikiuser.hpp>
#include <huggle_core/version.hpp>

static void testTalkPageWarningParser(QString id, QDate date, int level);
//! This is a unit test
class HuggleTest : public QObject
{
    Q_OBJECT

    public:
        HuggleTest();
        ~HuggleTest();

    private Q_SLOTS:
        void testCaseTalkPageParser0001() { testTalkPageWarningParser("0001", QDate(2014, 4, 1), 3); }
        void testCaseTalkPageParser0002() { testTalkPageWarningParser("0002", QDate(2014, 1, 22), 3); }
        void testCaseTalkPageParser0003() { testTalkPageWarningParser("0003", QDate(2014, 4, 2), 0); }
        void testCaseTalkPageParser0004() { testTalkPageWarningParser("0004", QDate(2014, 4, 2), 0); }
        void testCaseTalkPageParser0005() { testTalkPageWarningParser("0005", QDate(2014, 5, 7), 4); }
        void testCaseTalkPageParser0006() { testTalkPageWarningParser("0006", QDate(2014, 5, 7), 1); }
        void testCaseTalkPageParser0007() { testTalkPageWarningParser("0007", QDate(2014, 5, 4), 1); }
        void testCaseTalkPageParser0008() { testTalkPageWarningParser("0008", QDate(2014, 5, 10), 1); }
        void testCaseTalkPageParser0009() { testTalkPageWarningParser("0009", QDate(2014, 5, 10), 1); }
        void testCaseTalkPageParser0010() { testTalkPageWarningParser("0010", QDate(2014, 5, 12), 3); }
        void testCaseTalkPageParser0011() { testTalkPageWarningParser("0011", QDate(2014, 5, 10), 1); }
        void testCaseTalkPageParser0012() { testTalkPageWarningParser("0012", QDate(2014, 5, 10), 1); }
        void testCaseTalkPageParser0013() { testTalkPageWarningParser("0013", QDate(2014, 5, 13), 2); }
        void testCaseTalkPageParser0014() { testTalkPageWarningParser("0014", QDate(2014, 5, 13), 4); }
        void testCaseTalkPageParser0015() { testTalkPageWarningParser("0015", QDate(2014, 5, 16), 1); }
        //! Test if IsIP returns true for users who are IP's
        void testCaseWikiUserCheckIP();
        void testCaseTerminalParser();
        void testCaseConfigurationParse_YAML();
        void testCaseConfigurationParse_QL();
        void testCaseScores();
        void testCaseVersionComparison();
        void testCaseGenerics();
};

HuggleTest::HuggleTest()
{
    Huggle::Configuration::HuggleConfiguration = new Huggle::Configuration();
    QFile f(":/test/wikipage/config.txt");
    f.open(QIODevice::ReadOnly);
    Huggle::Configuration::HuggleConfiguration->Project = new Huggle::WikiSite("en", "en.wikipedia");
    Huggle::Configuration::HuggleConfiguration->ProjectConfig = new Huggle::ProjectConfiguration("enwiki");
    Huggle::Configuration::HuggleConfiguration->Project->ProjectConfig = Huggle::Configuration::HuggleConfiguration->ProjectConfig;
    Huggle::Configuration::HuggleConfiguration->Verbosity=10;
    Huggle::Configuration::HuggleConfiguration->ProjectConfig->Parse(f.readAll(), nullptr, Huggle::Configuration::HuggleConfiguration->Project);
    f.close();
}

HuggleTest::~HuggleTest()
{
    delete Huggle::Configuration::HuggleConfiguration;
}

void HuggleTest::testCaseConfigurationParse_YAML()
{
    QFile f(":/test/wikipage/config.yaml");
    f.open(QIODevice::ReadOnly);
    Huggle::WikiSite *temp = new Huggle::WikiSite("test", "test.wikipedia");
    Huggle::Configuration::HuggleConfiguration->Projects << temp;
    temp->ProjectConfig = new Huggle::ProjectConfiguration("test");
    QString error;
    temp->ProjectConfig->ParseYAML(f.readAll(), &error, temp);
    f.close();
    QVERIFY2(error.isEmpty(), QString(QString("Failed to parse configuration, error was: ") + error).toUtf8().data());
    QVERIFY2(temp->ProjectConfig->Ignores.count() == 3, "Invalid number of ignores");
    QVERIFY2(temp->ProjectConfig->WarningTypes.count() == 12, "Invalid number of warning types");
    QVERIFY2(temp->ProjectConfig->AlternativeMonths.count() == 12, "Invalid number of months");
    QVERIFY2(temp->ProjectConfig->Months.count() == 12, "Invalid number of months");
    QVERIFY2(temp->ProjectConfig->RevertSummaries.count() == 13, "Invalid number of revert-summaries");
}

void HuggleTest::testCaseConfigurationParse_QL()
{
    QString test = "sample-conf:hi\n\nlist1:\n  a,\nb\n";
    QStringList list = Huggle::HuggleParser::ConfigurationParse_QL("list1", test);
    QVERIFY2(list.count() == 2, "Invalid result for ConfigurationParse_QL, parsed wrong number of lines");
    test = "sample-conf:hi\n\nlist1:\n  a blab ldf xx.;g gfdsg,\nb,\n  c,\n          d d,\n";
    list = Huggle::HuggleParser::ConfigurationParse_QL("list1", test);
    QVERIFY2(list.count() == 4, "Invalid result for ConfigurationParse_QL, parsed wrong number of lines");
    QVERIFY2(list.at(2) == "c,", "Invalid result for ConfigurationParse_QL, parsed wrong item on position 3");
}

static void testTalkPageWarningParser(QString id, QDate date, int level)
{
    QFile *file = new QFile(":/test/wikipage/tp" + id + ".txt");
    Huggle::WikiUser *user = new Huggle::WikiUser(Huggle::Configuration::HuggleConfiguration->Project);
    file->open(QIODevice::ReadOnly);
    QString text = QString(file->readAll());
    user->TalkPage_SetContents(text);
    user->ParseTP(date);
    QVERIFY2(user->GetWarningLevel() == level, QString("level parsed was " + QString::number(user->GetWarningLevel()) + " should be " + QString::number(level) + "!!").toUtf8().data());
    file->close();
    delete file;
    delete user;

}

void HuggleTest::testCaseScores()
{
    Huggle::Configuration::HuggleConfiguration->ProjectConfig->ScoreWords.clear();
    Huggle::Configuration::HuggleConfiguration->ProjectConfig->ScoreWords.append(new Huggle::ScoreWord("fuck", 10));
    Huggle::Configuration::HuggleConfiguration->ProjectConfig->ScoreWords.append(new Huggle::ScoreWord("fucking", 20));
    Huggle::Configuration::HuggleConfiguration->ProjectConfig->ScoreWords.append(new Huggle::ScoreWord("vagina", 50));
    Huggle::Configuration::HuggleConfiguration->ProjectConfig->ScoreWords.append(new Huggle::ScoreWord("fuck this bitch", 20));
    Huggle::Configuration::HuggleConfiguration->ProjectConfig->ScoreWords.append(new Huggle::ScoreWord("suck", 60));
    Huggle::Configuration::HuggleConfiguration->ProjectConfig->ScoreWords.append(new Huggle::ScoreWord("ass", 60));
    Huggle::Configuration::HuggleConfiguration->SystemConfig_WordSeparators << " " << "." << "," << "(" << ")" << ":" << ";" << "!" << "?" << "/";
    Huggle::GC::gc = new Huggle::GC();
    Huggle::WikiEdit *edit = new Huggle::WikiEdit();
    edit->Page = new Huggle::WikiPage("test", hcfg->Project);
    edit->User = new Huggle::WikiUser("Harry, the vandal", Huggle::Configuration::HuggleConfiguration->Project);
    edit->DiffText = "fuck this vagina!";
    edit->Score = 0;
    edit->ProcessWords();
    QVERIFY2(edit->Score == 60, QString("01 Invalid result for score words: " + QString::number(edit->Score)).toUtf8().data());
    QVERIFY2(edit->ScoreWords.contains("vagina"), "02 Invalid result for score words");
    QVERIFY2(edit->ScoreWords.contains("fuck"), "03 Invalud result for score words");
    edit->DiffText = "fuc_k vagina!";
    edit->Score = 0;
    edit->ProcessWords();
    QVERIFY2(edit->Score == 50, QString("04 Invalid result for score words: " + QString::number(edit->Score)).toUtf8().data());
    QVERIFY2(edit->ScoreWords.contains("vagina"), "05 Invalid result for score words");
    edit->DiffText = "Hey bob, (fuck) there is some vagina.";
    edit->Score = 0;
    edit->ProcessWords();
    QVERIFY2(edit->Score == 60, "06 Invalid result for score words");
    QVERIFY2(edit->ScoreWords.contains("vagina"), "07 Invalid result for score words");
    QVERIFY2(edit->ScoreWords.contains("fuck"), "08 Invalid result for score words");
    edit->DiffText = "Hey bob, fuck this bitch over.";
    edit->Score = 0;
    edit->ProcessWords();
    QVERIFY2(edit->Score == 30, "06 Invalid result for score words");
    edit->DiffText = "Hey bob, (fuck) there is some vagina, let's fuck that vagina.";
    edit->Score = 0;
    edit->ProcessWords();
    QVERIFY2(edit->Score == 60, QString("09 Invalid result for score words: " + QString::number(edit->Score)).toUtf8().data());
    QVERIFY2(edit->ScoreWords.contains("vagina"), "10 Invalid result for score words");
    QVERIFY2(edit->ScoreWords.contains("fuck"), "12 Invalid result for score words");
    edit->DiffText = "Hey bob, there are vaginas over there";
    edit->Score = 0;
    edit->ProcessWords();
    QVERIFY2(edit->Score == 0, QString("14 Invalid result for score words: " + QString::number(edit->Score)).toUtf8().data());
    edit->DiffText = "Hey bob, mind if I'd be fucking with you?";
    edit->Score = 0;
    edit->ProcessWords();
    QVERIFY2(edit->Score == 20, QString("16 Invalid result for score words: " + QString::number(edit->Score)).toUtf8().data());
    edit->DiffText = "Hey bob, stop fuckin that vagina!!";
    edit->Score = 0;
    edit->ProcessWords();
    QVERIFY2(edit->Score == 50, QString("18 Invalid result for score words: " + QString::number(edit->Score)).toUtf8().data());
    QFile *vf = new QFile(":/test/wikipage/page01.txt");
    vf->open(QIODevice::ReadOnly);
    edit->DiffText = QString(vf->readAll());
    edit->Score = 0;
    edit->ProcessWords();
    delete vf;
    QVERIFY2(edit->Score == 0, QString("20 Invalid result for score words: " + QString::number(edit->Score)).toUtf8().data());
    vf = new QFile(":/test/wikipage/page02.txt");
    vf->open(QIODevice::ReadOnly);
    edit->DiffText = QString(vf->readAll());
    edit->Score = 0;
    edit->ProcessWords();
    delete vf;
    QVERIFY2(edit->Score == 0, QString("22 Invalid result for score words: " + QString::number(edit->Score)).toUtf8().data());
    vf = new QFile(":/test/wikipage/page03.txt");
    vf->open(QIODevice::ReadOnly);
    edit->DiffText = QString(vf->readAll());
    edit->Score = 0;
    edit->ProcessWords();
    delete vf;
    QVERIFY2(edit->Score == 60, QString("26 Invalid result for score words: " + QString::number(edit->Score)).toUtf8().data());
    vf = new QFile(":/test/wikipage/page04.txt");
    vf->open(QIODevice::ReadOnly);
    edit->DiffText = QString(vf->readAll());
    edit->Score = 0;
    edit->ProcessWords();
    delete vf;
    QVERIFY2(edit->Score == 10, QString("26 Invalid result for score words: " + QString::number(edit->Score)).toUtf8().data());
    edit->SafeDelete();
    Huggle::GC::gc->Stop();
    while (Huggle::GC::gc->IsRunning())
        Huggle::Sleeper::usleep(2);
    delete Huggle::GC::gc;
    Huggle::GC::gc = NULL;
}


void HuggleTest::testCaseWikiUserCheckIP()
{
    QVERIFY2(Huggle::WikiUser("10.0.0.1", hcfg->Project).IsIP(), "Invalid result for new WikiUser with username of 10.0.0.1, the result of IsIP() was false, but should have been true");
    QVERIFY2(Huggle::WikiUser("132.185.160.97", hcfg->Project).IsIP(), "Invalid result for new WikiUser with username of 132.185.160.97, the result of IsIP() was false, but should have been true");
    QVERIFY2(Huggle::WikiUser("150.30.0.56", hcfg->Project).IsIP(), "Invalid result for new WikiUser with username of 150.30.0.56, the result of IsIP() was false, but should have been true");
    QVERIFY2((Huggle::WikiUser("355.2.0.1", hcfg->Project).IsIP() == false), "Invalid result for new WikiUser with username of 355.2.0.1, the result of IsIP() was true, but should have been false");
    QVERIFY2((Huggle::WikiUser("Frank", hcfg->Project).IsIP() == false), "Invalid result for new WikiUser with username of IP, the result of IsIP() was true, but should have been false");
    QVERIFY2((Huggle::WikiUser("Joe", hcfg->Project).IsIP() == false), "Invalid result for new WikiUser with username of IP, the result of IsIP() was true, but should have been false");
    QVERIFY2((Huggle::WikiUser("2601:7:9380:135:1CCE:4CC0:7B6:8CD5", hcfg->Project).IsIP()), "Invalid result for new WikiUser with username of 2601:7:9380:135:1CCE:4CC0:7B6:8CD5, the result of IsIP() was false, but should have been true");
}

void HuggleTest::testCaseTerminalParser()
{
    QStringList list;
    list.append("huggle");
    list.append("-v");
    Huggle::TerminalParser *p = new Huggle::TerminalParser(list);
    p->Silent = true;
    QVERIFY2(p->Parse() == false, "Invalid result for terminal parser");
    list.append("-vvvvvvvvvvvvvvvvv");
    delete p;
    p = new Huggle::TerminalParser(list);
    p->Silent = true;
    QVERIFY2(p->Parse() == false, "Invalid result for terminal parser");
    list.append("-vvvvvvhvvvvvvv");
    delete p;
    p = new Huggle::TerminalParser(list);
    p->Silent = true;
    QVERIFY2(p->Parse() == true, "Invalid result for terminal parser");
    list.clear();
    list.append("huggle");
    list.append("--help");
    delete p;
    p = new Huggle::TerminalParser(list);
    p->Silent = true;
    QVERIFY2(p->Parse() == true, "Invalid result for terminal parser");
    list.clear();
    list.append("huggle");
    list.append("--safe");
    delete p;
    p = new Huggle::TerminalParser(list);
    p->Silent = true;
    QVERIFY2(p->Parse() == false, "Invalid result for terminal parser");
    list.clear();
    list.append("huggle");
    list.append("--blabla");
    delete p;
    p = new Huggle::TerminalParser(list);
    p->Silent = true;
    QVERIFY2(p->Parse() == true, "Invalid result for terminal parser");
    delete p;
}

void HuggleTest::testCaseVersionComparison()
{
    QVERIFY2(Huggle::Version("1.2.0") > Huggle::Version("1.0.0"), "Invalid version comparison results: fail");
    QVERIFY2(Huggle::Version("3.1.1") > Huggle::Version("1.8.0-wmf1"), "Invalid version comparison results: fail");
    QVERIFY2(Huggle::Version("3.0.1") > Huggle::Version("1.6.2"), "Invalid version comparison results: fail");
    QVERIFY2(Huggle::Version("4.2.0") > Huggle::Version("1.0.0.5"), "Invalid version comparison results: fail");
    QVERIFY2(Huggle::Version("1.2.0.1") > Huggle::Version("1.0.0"), "Invalid version comparison results: fail");
    QVERIFY2(Huggle::Version("1.2.0") >= Huggle::Version("1.0.0"), "Invalid version comparison results: fail");
    QVERIFY2(Huggle::Version("1.2.5") < Huggle::Version("1.4.0-wmf1"), "Invalid version comparison results: fail");
    QVERIFY2(Huggle::Version("1.2.0") <= Huggle::Version("1.2.0"), "Invalid version comparison results: fail");
    QVERIFY2(Huggle::Version("1.2.0") == Huggle::Version("1.2.0"), "Invalid version comparison results: fail");
    QVERIFY2(Huggle::Version("1.2.0") != Huggle::Version("2.0"), "Invalid version comparison results: fail");
    QVERIFY2(Huggle::Version("1.2") > Huggle::Version("1.0.0"), "Invalid version comparison results: fail");
    QVERIFY2(Huggle::Version("1.2.0") > Huggle::Version("1.0.2.5462"), "Invalid version comparison results: fail");
}

void HuggleTest::testCaseGenerics()
{
    QVERIFY2(Huggle::Generic::Bool2String(true).toLower() == "true", "Invalid conversion of bool to string");
    QVERIFY2(Huggle::Generic::Bool2String(false).toLower() == "false", "Invalid conversion of bool to string");
    QVERIFY2(Huggle::Generic::SafeBool("f", false) == false, "Invalid conversion of string to bool");
    QVERIFY2(Huggle::Generic::SafeBool("x", true) == true, "Invalid conversion of string to bool");
    QVERIFY2(Huggle::Generic::SafeBool("x", false) == false, "Invalid conversion of string to bool");
    QVERIFY2(Huggle::Generic::SafeBool("true") == true, "Invalid conversion of string to bool");
    QVERIFY2(Huggle::Generic::SafeBool("false") == false, "Invalid conversion of string to bool");
    QVERIFY2(Huggle::Generic::SafeBool("TRUE") == true, "Invalid conversion of string to bool");
}

QTEST_APPLESS_MAIN(HuggleTest)

#include "tst_testmain.moc"
