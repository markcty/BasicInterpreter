#include "environment.h"

int Environment::getIntValue(const QString &variable) const {
  if (!env.contains(variable))
    throw QStringException(variable + " does not exist in runtime environment");
  return env[variable].intVal;
}

QString Environment::getStrValue(const QString &variable) const {
  return env[variable].strVal;
}

Environment::Type Environment::getType(const QString &variable) const {
  return env[variable].type;
}

void Environment::setValue(const QString &variable, int value) {
  env[variable].intVal = value;
}

void Environment::setValue(const QString &variable, const QString &value) {
  env[variable].strVal = value;
}

void Environment::clear() { env.clear(); }
