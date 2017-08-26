/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月23日 星期三 22时36分24秒
 @File Name: pb_helper.cpp
 @Description:
 ******************************************************/

#include "str_helper/str_helper.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <string>

using namespace google::protobuf;

using namespace utils::str_helper;

namespace utils {
namespace pb_helper {

namespace {
#define TypeFieldToString(type)                                               \
	std::string type##FieldToString(const Reflection* reflection,             \
									const FieldDescriptor* field,             \
									const Message* message) {                 \
		std::string result;                                                   \
		result.append(valueToQuotedString(field->name().data()));             \
		result.append(":");                                                   \
		result.append(valueToString(reflection->Get##type(*message, field))); \
		return result;                                                        \
	}
TypeFieldToString(Int32);
TypeFieldToString(Int64);
TypeFieldToString(UInt32);
TypeFieldToString(UInt64);
TypeFieldToString(Float);
TypeFieldToString(Double);
TypeFieldToString(Bool);
TypeFieldToString(EnumValue);
std::string StringFieldToString(const Reflection* reflection,
								const FieldDescriptor* field,
								const Message* message) {
	std::string result;
	result.append(valueToQuotedString(field->name().data()));
	result.append(":");
	result.append(valueToQuotedString(reflection->GetString(*message, field)));
	return result;
}
// TODO message
}

namespace {
/** Repeated macro **/
#define RepeatedTypeFieldToString(type)                                     \
	std::string Repeated##type##FieldToString(const Reflection* reflection, \
											  const FieldDescriptor* field, \
											  const Message* message) {     \
		std::string result;                                                 \
		result.append(valueToQuotedString(field->name().data()));           \
		result.append(":");                                                 \
		result.append("[");                                                 \
		int field_size = reflection->FieldSize(*message, field);            \
		for (int index = 0; index < field_size; ++index) {                  \
			result.append(valueToString(                                    \
				reflection->GetRepeated##type(*message, field, index)));    \
			if (index != field_size-1) {                                      \
				result.append(",");                                         \
			}                                                               \
		}                                                                   \
		result.append("]");                                                 \
		return result;                                                      \
	};
RepeatedTypeFieldToString(Int32);
RepeatedTypeFieldToString(Int64);
RepeatedTypeFieldToString(UInt32);
RepeatedTypeFieldToString(UInt64);
RepeatedTypeFieldToString(Float);
RepeatedTypeFieldToString(Double);
RepeatedTypeFieldToString(Bool);
RepeatedTypeFieldToString(EnumValue);
std::string RepeatedStringFieldToString(const Reflection* reflection,
										const FieldDescriptor* field,
										const Message* message) {
	std::string result;
	result.append(valueToQuotedString(field->name().data()));
	result.append(":");
	result.append("[");
	int field_size = reflection->FieldSize(*message, field);
	for (int index = 0; index < field_size; ++index) {
		result.append(valueToQuotedString(
			reflection->GetRepeatedString(*message, field, index)));
		if (index != field_size-1) {
			result.append(",");
		}
	}
	result.append("]");
	return result;
}
// TODO message
}

std::string pb2Json(const Message* message) {
	std::string result;
	result.append("{");
	const Descriptor* descriptor = message->GetDescriptor();
	int nfields = descriptor->field_count();
	const Reflection* reflection = message->GetReflection();
	for (int index = 0; index < nfields; ++index) {
		const FieldDescriptor* field = descriptor->field(index);
		FieldDescriptor::Type type = field->type();
		FieldDescriptor::Label label = field->label();
		switch (type) {
			case FieldDescriptor::TYPE_DOUBLE:
				if (label == FieldDescriptor::LABEL_REPEATED) {
				} else {
					result += DoubleFieldToString(reflection, field, message);
				}
				break;
			case FieldDescriptor::TYPE_FLOAT:
				break;
			case FieldDescriptor::TYPE_INT64:
				break;
			case FieldDescriptor::TYPE_UINT64:
				break;
			case FieldDescriptor::TYPE_INT32:
				if (label == FieldDescriptor::LABEL_REPEATED) {
					result +=
						RepeatedInt32FieldToString(reflection, field, message);
				} else {
					result += Int32FieldToString(reflection, field, message);
				}
				break;
			case FieldDescriptor::TYPE_FIXED64:
				break;
			case FieldDescriptor::TYPE_FIXED32:
				break;
			case FieldDescriptor::TYPE_BOOL:
				break;
			case FieldDescriptor::TYPE_STRING:
				if (label == FieldDescriptor::LABEL_REPEATED) {
					result +=
						RepeatedStringFieldToString(reflection, field, message);
				} else {
					result += StringFieldToString(reflection, field, message);
				}
				break;
			case FieldDescriptor::TYPE_GROUP:
				// Deprecated
				break;
			case FieldDescriptor::TYPE_MESSAGE:
				break;
			case FieldDescriptor::TYPE_BYTES:
				break;
			case FieldDescriptor::TYPE_UINT32:
				break;
			case FieldDescriptor::TYPE_ENUM:
				break;
			case FieldDescriptor::TYPE_SFIXED32:
				break;
			case FieldDescriptor::TYPE_SFIXED64:
				break;
			case FieldDescriptor::TYPE_SINT32:
				break;
			case FieldDescriptor::TYPE_SINT64:
				break;
			default:
				break;
		}

		if (index != nfields-1) {
			result.append(",");
		}
	}
	result.append("}");
	return result;
}
}
}
