/**
 * Some de-bullshitting for Qt JSON failures.
 *
 * Simple exception-throwing
 */

#pragma once
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "MMCError.h"

class JSONValidationError : public MMCError
{
public:
	JSONValidationError(QString cause) : MMCError(cause) {};
	virtual ~JSONValidationError() noexcept {}
};

namespace MMCJson
{
/// make sure the value exists. throw otherwise.
QJsonValue ensureExists(QJsonValue val, const QString what = "value");

/// make sure the value is converted into an object. throw otherwise.
QJsonObject ensureObject(const QJsonValue val, const QString what = "value");

/// make sure the document is converted into an object. throw otherwise.
QJsonObject ensureObject(const QJsonDocument val, const QString what = "value");

/// make sure the value is converted into an array. throw otherwise.
QJsonArray ensureArray(const QJsonValue val, QString what = "value");

/// make sure the value is converted into a string. throw otherwise.
QString ensureString(const QJsonValue val, QString what = "value");

/// make sure the value is converted into a boolean. throw otherwise.
bool ensureBoolean(const QJsonValue val, QString what = "value");

/// make sure the value is converted into an integer. throw otherwise.
int ensureInteger(const QJsonValue val, QString what = "value");

/// make sure the value is converted into a double precision floating number. throw otherwise.
double ensureDouble(const QJsonValue val, QString what = "value");

void writeString(QJsonObject & to, QString key, QString value);

void writeStringList (QJsonObject & to, QString key, QStringList values);

template <typename T>
void writeObjectList (QJsonObject & to, QString key, QList<T> values)
{
	if(values.size())
	{
		QJsonArray array;
		for(auto value: values)
		{
			array.append(value->toJson());
		}
		to.insert(key, array);
	}
}
}

