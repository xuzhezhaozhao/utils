/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月23日 星期三 22时36分24秒
 @File Name: pb_helper.cpp
 @Description:
 @Dependency: 依赖 str_helper 模块
 ******************************************************/

#include "str_helper/str_helper.hpp"
#include "pb_helper.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <string>

using namespace google::protobuf;
using namespace utils::str_helper;

// forward declaration
static std::string __protobufToJsonString(const Message& msg);

namespace {
// field to string macro
#define DEFINE_TypeFieldToString(type, func)                         \
	std::string type##FieldToString(const Reflection* reflection,    \
									const FieldDescriptor* field,    \
									const Message* message) {        \
		std::string result;                                          \
		result.append(valueToQuotedString(field->name().data()));    \
		result.append(":");                                          \
		result.append(func(reflection->Get##type(*message, field))); \
		return result;                                               \
	}
DEFINE_TypeFieldToString(Int32, valueToString);
DEFINE_TypeFieldToString(Int64, valueToString);
DEFINE_TypeFieldToString(UInt32, valueToString);
DEFINE_TypeFieldToString(UInt64, valueToString);
DEFINE_TypeFieldToString(Float, valueToString);
DEFINE_TypeFieldToString(Double, valueToString);
DEFINE_TypeFieldToString(Bool, valueToString);
DEFINE_TypeFieldToString(EnumValue, valueToString);
DEFINE_TypeFieldToString(String, valueToQuotedString);
DEFINE_TypeFieldToString(Message, __protobufToJsonString);

#undef DEFINE_TypeFieldToString
}


namespace {
/** Repeated field to string macro **/
#define DEFINE_RepeatedTypeFieldToString(type, func)                          \
	std::string Repeated##type##FieldToString(const Reflection* reflection,   \
											  const FieldDescriptor* field,   \
											  const Message* message) {       \
		std::string result;                                                   \
		result.append(valueToQuotedString(field->name().data()));             \
		result.append(":");                                                   \
		result.append("[");                                                   \
		int field_size = reflection->FieldSize(*message, field);              \
		for (int index = 0; index < field_size; ++index) {                    \
			result.append(                                                    \
				func(reflection->GetRepeated##type(*message, field, index))); \
			if (index != field_size - 1) {                                    \
				result.append(",");                                           \
			}                                                                 \
		}                                                                     \
		result.append("]");                                                   \
		return result;                                                        \
	};
DEFINE_RepeatedTypeFieldToString(Int32, valueToString);
DEFINE_RepeatedTypeFieldToString(Int64, valueToString);
DEFINE_RepeatedTypeFieldToString(UInt32, valueToString);
DEFINE_RepeatedTypeFieldToString(UInt64, valueToString);
DEFINE_RepeatedTypeFieldToString(Float, valueToString);
DEFINE_RepeatedTypeFieldToString(Double, valueToString);
DEFINE_RepeatedTypeFieldToString(Bool, valueToString);
DEFINE_RepeatedTypeFieldToString(EnumValue, valueToString);
DEFINE_RepeatedTypeFieldToString(String, valueToQuotedString);
DEFINE_RepeatedTypeFieldToString(Message, __protobufToJsonString);
}

namespace {
#define DEFINE_FieldAppendToString(type)                                   \
	void type##FieldAppendToString(                                        \
		std::string& result, const Reflection* reflection,                 \
		const FieldDescriptor* field, const Message* message) {            \
		FieldDescriptor::Label label = field->label();                     \
		if (label == FieldDescriptor::LABEL_REPEATED) {                    \
			result +=                                                      \
				Repeated##type##FieldToString(reflection, field, message); \
		} else {                                                           \
			result += type##FieldToString(reflection, field, message);     \
		}                                                                  \
	}
DEFINE_FieldAppendToString(Int32);
DEFINE_FieldAppendToString(Int64);
DEFINE_FieldAppendToString(UInt32);
DEFINE_FieldAppendToString(UInt64);
DEFINE_FieldAppendToString(Float);
DEFINE_FieldAppendToString(Double);
DEFINE_FieldAppendToString(Bool);
DEFINE_FieldAppendToString(EnumValue);
DEFINE_FieldAppendToString(String);
DEFINE_FieldAppendToString(Message);
}

// 使用引用参数原因是 Message::GetMessage() 方法返回的是 const Message& 
static std::string __protobufToJsonString(const Message& msg) {
	const Message* message = &msg;
	std::string result;
	result.append("{");
	const Descriptor* descriptor = message->GetDescriptor();
	int nfields = descriptor->field_count();
	const Reflection* reflection = message->GetReflection();
	for (int index = 0; index < nfields; ++index) {
		const FieldDescriptor* field = descriptor->field(index);
		FieldDescriptor::Type type = field->type();
		switch (type) {
			case FieldDescriptor::TYPE_DOUBLE:
				DoubleFieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_FLOAT:
				FloatFieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_INT64:
				Int64FieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_UINT64:
				UInt64FieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_INT32:
				Int32FieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_FIXED64:
				UInt64FieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_FIXED32:
				UInt32FieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_BOOL:
				BoolFieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_STRING:
				StringFieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_GROUP:
				// TODO Deprecated
				break;
			case FieldDescriptor::TYPE_MESSAGE:
				MessageFieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_BYTES:
				StringFieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_UINT32:
				UInt32FieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_ENUM:
				EnumValueFieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_SFIXED32:
				Int32FieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_SFIXED64:
				Int64FieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_SINT32:
				Int32FieldAppendToString(result, reflection, field, message);
				break;
			case FieldDescriptor::TYPE_SINT64:
				Int64FieldAppendToString(result, reflection, field, message);
				break;
			default:
				break;
		}

		if (index != nfields - 1) {
			result.append(",");
		}
	}
	result.append("}");
	return result;
}

namespace utils {
namespace pb_helper {

std::string protobufToJsonString(const Message* message) {
	return __protobufToJsonString(*message);
}

}
}
