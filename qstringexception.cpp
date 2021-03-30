#include "qstringexception.h"

QStringException::QStringException(const QString &s) : message(s) {}

QString QStringException::what() const { return message; }
