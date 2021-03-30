#include "environment.h"

int Environment::getValue(const QString &variable) const {
  if (!env.contains(variable))
    throw QStringException(variable + " does not exist in runtime environment");
  return env[variable];
}
void Environment::setValue(const QString &variable, int value) {
  env[variable] = value;
}

void Environment::clear() { env.clear(); }
