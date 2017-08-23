/*******************************************************
 @Author: zhezhaoxu
 @Created Time : 2017年08月23日 星期三 22时36分24秒
 @File Name: pb_helper.cpp
 @Description:
 ******************************************************/

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <string>

using namespace google::protobuf;

namespace utils
{
namespace pb_helper
{

std::string pb2Json(const Message *message) {
	std::string result;
	const Descriptor* descriptor = message->GetDescriptor();
	int nfields = descriptor->field_count();
	const Reflection* reflection = message->GetReflection();
	for (int i = 0; i < nfields; ++i) {
		const FieldDescriptor* field = descriptor->field(i);
		FieldDescriptor::Type type = field->type();
		FieldDescriptor::Label label = field->label();
		if (field->label() == FieldDescriptor::LABEL_REPEATED) {
			if (reflection->FieldSize(*message, field) == 0) {
				continue;
			}
		} else {
			if (!reflection->HasField(*message, field)) {
				continue;
			}
		}

		switch (type) {
			case FieldDescriptor::TYPE_DOUBLE:
				if (label == FieldDescriptor::LABEL_REPEATED) {
				} else {
					// FileDescriptor::LABEL_OPTIONAL 
					// or FileDescriptor::LABEL_REQUIRED 
				}
				break;
			case FieldDescriptor::TYPE_FLOAT:
				break;
			case FieldDescriptor::TYPE_INT64:
				break;
			case FieldDescriptor::TYPE_UINT64:
				break;
			case FieldDescriptor::TYPE_INT32:
				break;
			case FieldDescriptor::TYPE_FIXED64:
				break;
			case FieldDescriptor::TYPE_FIXED32:
				break;
			case FieldDescriptor::TYPE_BOOL:
				break;
			case FieldDescriptor::TYPE_STRING:
				if (label == FieldDescriptor::LABEL_REPEATED) {

				} else {
					// FileDescriptor::LABEL_OPTIONAL 
					// or FileDescriptor::LABEL_REQUIRED 
					result += "\"" + field->json_name() + "\"" + ":" 
						+ "\"" + reflection->GetString(*message, field) + "\"";
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
	}
	return result;
}

}
}
