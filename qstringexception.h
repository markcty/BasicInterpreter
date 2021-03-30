#ifndef QSTRINGEXCEPTION_H
#define QSTRINGEXCEPTION_H

#include <QString>

class QStringException {
 public:
  QString message;
  QStringException(const QString &s);
  QString what() const;
};

#endif  // QSTRINGEXCEPTION_H
