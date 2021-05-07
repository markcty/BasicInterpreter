#include "environment.h"

int Environment::getIntValue(const QString &variable) const {
  if (!env.contains(variable))
    throw QStringException(variable + " does not exist in runtime environment");
  return env[variable].intVal;
}

QString Environment::getStrValue(const QString &variable) const {
  return env[variable].strVal;
}

VariableType Environment::getType(const QString &variable) const {
  return env[variable].type;
}

void Environment::setValue(const QString &variable, int value) {
  env[variable].intVal = value;
  env[variable].type = INT;
}

void Environment::setValue(const QString &variable, const QString &value) {
  env[variable].strVal = value;
  env[variable].type = STR;
}

void Environment::clear() { env.clear(); }

QString Environment::toString() const {
  QString s;
  for (auto i = env.constBegin(); i != env.constEnd(); i++) {
    if (i.value().type == INT)
      s += i.key() + ": INT = " + QString::number(i.value().intVal);
    if (i.value().type == STR)
      s += i.key() + ": STR = \"" + i.value().strVal + "\"";
    s += "\n";
  }
  return s;
}
