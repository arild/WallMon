// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "FastLoggerProtocol.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace FastLogger {

namespace {

const ::google::protobuf::Descriptor* DataPacket_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  DataPacket_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_FastLoggerProtocol_2eproto() {
  protobuf_AddDesc_FastLoggerProtocol_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "FastLoggerProtocol.proto");
  GOOGLE_CHECK(file != NULL);
  DataPacket_descriptor_ = file->message_type(0);
  static const int DataPacket_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DataPacket, junk_),
  };
  DataPacket_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      DataPacket_descriptor_,
      DataPacket::default_instance_,
      DataPacket_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DataPacket, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DataPacket, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(DataPacket));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_FastLoggerProtocol_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    DataPacket_descriptor_, &DataPacket::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_FastLoggerProtocol_2eproto() {
  delete DataPacket::default_instance_;
  delete DataPacket_reflection_;
}

void protobuf_AddDesc_FastLoggerProtocol_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\030FastLoggerProtocol.proto\022\nFastLogger\"\032"
    "\n\nDataPacket\022\014\n\004junk\030\001 \002(\014", 66);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "FastLoggerProtocol.proto", &protobuf_RegisterTypes);
  DataPacket::default_instance_ = new DataPacket();
  DataPacket::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_FastLoggerProtocol_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_FastLoggerProtocol_2eproto {
  StaticDescriptorInitializer_FastLoggerProtocol_2eproto() {
    protobuf_AddDesc_FastLoggerProtocol_2eproto();
  }
} static_descriptor_initializer_FastLoggerProtocol_2eproto_;


// ===================================================================

const ::std::string DataPacket::_default_junk_;
#ifndef _MSC_VER
const int DataPacket::kJunkFieldNumber;
#endif  // !_MSC_VER

DataPacket::DataPacket()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void DataPacket::InitAsDefaultInstance() {
}

DataPacket::DataPacket(const DataPacket& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void DataPacket::SharedCtor() {
  _cached_size_ = 0;
  junk_ = const_cast< ::std::string*>(&_default_junk_);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

DataPacket::~DataPacket() {
  SharedDtor();
}

void DataPacket::SharedDtor() {
  if (junk_ != &_default_junk_) {
    delete junk_;
  }
  if (this != default_instance_) {
  }
}

void DataPacket::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* DataPacket::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return DataPacket_descriptor_;
}

const DataPacket& DataPacket::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_FastLoggerProtocol_2eproto();  return *default_instance_;
}

DataPacket* DataPacket::default_instance_ = NULL;

DataPacket* DataPacket::New() const {
  return new DataPacket;
}

void DataPacket::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (_has_bit(0)) {
      if (junk_ != &_default_junk_) {
        junk_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool DataPacket::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required bytes junk = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_junk()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void DataPacket::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required bytes junk = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      1, this->junk(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* DataPacket::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required bytes junk = 1;
  if (_has_bit(0)) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        1, this->junk(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int DataPacket::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required bytes junk = 1;
    if (has_junk()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->junk());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void DataPacket::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const DataPacket* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const DataPacket*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void DataPacket::MergeFrom(const DataPacket& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_junk(from.junk());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void DataPacket::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void DataPacket::CopyFrom(const DataPacket& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool DataPacket::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  return true;
}

void DataPacket::Swap(DataPacket* other) {
  if (other != this) {
    std::swap(junk_, other->junk_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata DataPacket::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = DataPacket_descriptor_;
  metadata.reflection = DataPacket_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace FastLogger

// @@protoc_insertion_point(global_scope)