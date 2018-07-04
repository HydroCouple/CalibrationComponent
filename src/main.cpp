#include "stdafx.h"
#include <QCoreApplication>
#include <QCommandLineParser>

int main(int argc, char** argv)
{

  qputenv("QTEST_FUNCTION_TIMEOUT", "1000000000");

  return 0;
}
