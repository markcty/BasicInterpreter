#include "environment.h"

int Environment::getValue(const QString &variable) const {
  return env[variable];
}
void Environment::setValue(const QString &variable, int value) {
  env[variable] = value;
}

void Environment::clear() { env.clear(); }
