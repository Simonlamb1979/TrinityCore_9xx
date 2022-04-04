// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: club_ban.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "club_ban.pb.h"

#include <algorithm>
#include <utility>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
#include "Log.h"
// @@protoc_insertion_point(includes)

namespace bgs {
namespace protocol {
namespace club {
namespace v1 {

namespace {

const ::google::protobuf::Descriptor* AddBanOptions_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  AddBanOptions_reflection_ = NULL;
const ::google::protobuf::Descriptor* ClubBan_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ClubBan_reflection_ = NULL;

}  // namespace

void protobuf_AssignDesc_club_5fban_2eproto() {
  protobuf_AddDesc_club_5fban_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "club_ban.proto");
  GOOGLE_CHECK(file != NULL);
  AddBanOptions_descriptor_ = file->message_type(0);
  static const int AddBanOptions_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AddBanOptions, target_id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AddBanOptions, attribute_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AddBanOptions, reason_),
  };
  AddBanOptions_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      AddBanOptions_descriptor_,
      AddBanOptions::default_instance_,
      AddBanOptions_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AddBanOptions, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AddBanOptions, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(AddBanOptions));
  ClubBan_descriptor_ = file->message_type(1);
  static const int ClubBan_offsets_[6] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClubBan, id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClubBan, battle_tag_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClubBan, creator_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClubBan, attribute_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClubBan, reason_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClubBan, creation_time_),
  };
  ClubBan_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ClubBan_descriptor_,
      ClubBan::default_instance_,
      ClubBan_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClubBan, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClubBan, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ClubBan));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_club_5fban_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    AddBanOptions_descriptor_, &AddBanOptions::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ClubBan_descriptor_, &ClubBan::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_club_5fban_2eproto() {
  delete AddBanOptions::default_instance_;
  delete AddBanOptions_reflection_;
  delete ClubBan::default_instance_;
  delete ClubBan_reflection_;
}

void protobuf_AddDesc_club_5fban_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::bgs::protocol::protobuf_AddDesc_global_5fextensions_2ffield_5foptions_2eproto();
  ::bgs::protocol::club::v1::protobuf_AddDesc_club_5fmember_2eproto();
  ::bgs::protocol::v2::protobuf_AddDesc_api_2fclient_2fv2_2fattribute_5ftypes_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\016club_ban.proto\022\024bgs.protocol.club.v1\032%"
    "global_extensions/field_options.proto\032\021c"
    "lub_member.proto\032#api/client/v2/attribut"
    "e_types.proto\"\201\001\n\rAddBanOptions\0221\n\ttarge"
    "t_id\030\001 \001(\0132\036.bgs.protocol.club.v1.Member"
    "Id\022-\n\tattribute\030\002 \003(\0132\032.bgs.protocol.v2."
    "Attribute\022\016\n\006reason\030\003 \001(\t\"\341\001\n\007ClubBan\022*\n"
    "\002id\030\001 \001(\0132\036.bgs.protocol.club.v1.MemberI"
    "d\022\032\n\nbattle_tag\030\002 \001(\tB\006\202\371+\002\010\001\0228\n\007creator"
    "\030\003 \001(\0132\'.bgs.protocol.club.v1.MemberDesc"
    "ription\022-\n\tattribute\030\004 \003(\0132\032.bgs.protoco"
    "l.v2.Attribute\022\016\n\006reason\030\005 \001(\t\022\025\n\rcreati"
    "on_time\030\006 \001(\004B\002H\001", 497);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "club_ban.proto", &protobuf_RegisterTypes);
  AddBanOptions::default_instance_ = new AddBanOptions();
  ClubBan::default_instance_ = new ClubBan();
  AddBanOptions::default_instance_->InitAsDefaultInstance();
  ClubBan::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_club_5fban_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_club_5fban_2eproto {
  StaticDescriptorInitializer_club_5fban_2eproto() {
    protobuf_AddDesc_club_5fban_2eproto();
  }
} static_descriptor_initializer_club_5fban_2eproto_;
// ===================================================================

#ifndef _MSC_VER
const int AddBanOptions::kTargetIdFieldNumber;
const int AddBanOptions::kAttributeFieldNumber;
const int AddBanOptions::kReasonFieldNumber;
#endif  // !_MSC_VER

AddBanOptions::AddBanOptions()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:bgs.protocol.club.v1.AddBanOptions)
}

void AddBanOptions::InitAsDefaultInstance() {
  target_id_ = const_cast< ::bgs::protocol::club::v1::MemberId*>(&::bgs::protocol::club::v1::MemberId::default_instance());
}

AddBanOptions::AddBanOptions(const AddBanOptions& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:bgs.protocol.club.v1.AddBanOptions)
}

void AddBanOptions::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  target_id_ = NULL;
  reason_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

AddBanOptions::~AddBanOptions() {
  // @@protoc_insertion_point(destructor:bgs.protocol.club.v1.AddBanOptions)
  SharedDtor();
}

void AddBanOptions::SharedDtor() {
  if (reason_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete reason_;
  }
  if (this != default_instance_) {
    delete target_id_;
  }
}

void AddBanOptions::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* AddBanOptions::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return AddBanOptions_descriptor_;
}

const AddBanOptions& AddBanOptions::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_club_5fban_2eproto();
  return *default_instance_;
}

AddBanOptions* AddBanOptions::default_instance_ = NULL;

AddBanOptions* AddBanOptions::New() const {
  return new AddBanOptions;
}

void AddBanOptions::Clear() {
  if (_has_bits_[0 / 32] & 5) {
    if (has_target_id()) {
      if (target_id_ != NULL) target_id_->::bgs::protocol::club::v1::MemberId::Clear();
    }
    if (has_reason()) {
      if (reason_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        reason_->clear();
      }
    }
  }
  attribute_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool AddBanOptions::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:bgs.protocol.club.v1.AddBanOptions)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional .bgs.protocol.club.v1.MemberId target_id = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_target_id()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_attribute;
        break;
      }

      // repeated .bgs.protocol.v2.Attribute attribute = 2;
      case 2: {
        if (tag == 18) {
         parse_attribute:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_attribute()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_attribute;
        if (input->ExpectTag(26)) goto parse_reason;
        break;
      }

      // optional string reason = 3;
      case 3: {
        if (tag == 26) {
         parse_reason:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_reason()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->reason().data(), this->reason().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "reason");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:bgs.protocol.club.v1.AddBanOptions)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:bgs.protocol.club.v1.AddBanOptions)
  return false;
#undef DO_
}

void AddBanOptions::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:bgs.protocol.club.v1.AddBanOptions)
  // optional .bgs.protocol.club.v1.MemberId target_id = 1;
  if (has_target_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->target_id(), output);
  }

  // repeated .bgs.protocol.v2.Attribute attribute = 2;
  for (int i = 0; i < this->attribute_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      2, this->attribute(i), output);
  }

  // optional string reason = 3;
  if (has_reason()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->reason().data(), this->reason().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "reason");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      3, this->reason(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:bgs.protocol.club.v1.AddBanOptions)
}

::google::protobuf::uint8* AddBanOptions::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:bgs.protocol.club.v1.AddBanOptions)
  // optional .bgs.protocol.club.v1.MemberId target_id = 1;
  if (has_target_id()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->target_id(), target);
  }

  // repeated .bgs.protocol.v2.Attribute attribute = 2;
  for (int i = 0; i < this->attribute_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        2, this->attribute(i), target);
  }

  // optional string reason = 3;
  if (has_reason()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->reason().data(), this->reason().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "reason");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->reason(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:bgs.protocol.club.v1.AddBanOptions)
  return target;
}

int AddBanOptions::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional .bgs.protocol.club.v1.MemberId target_id = 1;
    if (has_target_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->target_id());
    }

    // optional string reason = 3;
    if (has_reason()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->reason());
    }

  }
  // repeated .bgs.protocol.v2.Attribute attribute = 2;
  total_size += 1 * this->attribute_size();
  for (int i = 0; i < this->attribute_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->attribute(i));
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

void AddBanOptions::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const AddBanOptions* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const AddBanOptions*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void AddBanOptions::MergeFrom(const AddBanOptions& from) {
  GOOGLE_CHECK_NE(&from, this);
  attribute_.MergeFrom(from.attribute_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_target_id()) {
      mutable_target_id()->::bgs::protocol::club::v1::MemberId::MergeFrom(from.target_id());
    }
    if (from.has_reason()) {
      set_reason(from.reason());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void AddBanOptions::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void AddBanOptions::CopyFrom(const AddBanOptions& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool AddBanOptions::IsInitialized() const {
  if (has_target_id()) {
    if (!this->target_id().IsInitialized()) return false;
  }
  return true;
}

void AddBanOptions::Swap(AddBanOptions* other) {
  if (other != this) {
    std::swap(target_id_, other->target_id_);
    attribute_.Swap(&other->attribute_);
    std::swap(reason_, other->reason_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata AddBanOptions::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = AddBanOptions_descriptor_;
  metadata.reflection = AddBanOptions_reflection_;
  return metadata;
}

// ===================================================================

#ifndef _MSC_VER
const int ClubBan::kIdFieldNumber;
const int ClubBan::kBattleTagFieldNumber;
const int ClubBan::kCreatorFieldNumber;
const int ClubBan::kAttributeFieldNumber;
const int ClubBan::kReasonFieldNumber;
const int ClubBan::kCreationTimeFieldNumber;
#endif  // !_MSC_VER

ClubBan::ClubBan()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:bgs.protocol.club.v1.ClubBan)
}

void ClubBan::InitAsDefaultInstance() {
  id_ = const_cast< ::bgs::protocol::club::v1::MemberId*>(&::bgs::protocol::club::v1::MemberId::default_instance());
  creator_ = const_cast< ::bgs::protocol::club::v1::MemberDescription*>(&::bgs::protocol::club::v1::MemberDescription::default_instance());
}

ClubBan::ClubBan(const ClubBan& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:bgs.protocol.club.v1.ClubBan)
}

void ClubBan::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  id_ = NULL;
  battle_tag_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  creator_ = NULL;
  reason_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  creation_time_ = GOOGLE_ULONGLONG(0);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ClubBan::~ClubBan() {
  // @@protoc_insertion_point(destructor:bgs.protocol.club.v1.ClubBan)
  SharedDtor();
}

void ClubBan::SharedDtor() {
  if (battle_tag_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete battle_tag_;
  }
  if (reason_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete reason_;
  }
  if (this != default_instance_) {
    delete id_;
    delete creator_;
  }
}

void ClubBan::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ClubBan::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ClubBan_descriptor_;
}

const ClubBan& ClubBan::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_club_5fban_2eproto();
  return *default_instance_;
}

ClubBan* ClubBan::default_instance_ = NULL;

ClubBan* ClubBan::New() const {
  return new ClubBan;
}

void ClubBan::Clear() {
  if (_has_bits_[0 / 32] & 55) {
    if (has_id()) {
      if (id_ != NULL) id_->::bgs::protocol::club::v1::MemberId::Clear();
    }
    if (has_battle_tag()) {
      if (battle_tag_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        battle_tag_->clear();
      }
    }
    if (has_creator()) {
      if (creator_ != NULL) creator_->::bgs::protocol::club::v1::MemberDescription::Clear();
    }
    if (has_reason()) {
      if (reason_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        reason_->clear();
      }
    }
    creation_time_ = GOOGLE_ULONGLONG(0);
  }
  attribute_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ClubBan::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:bgs.protocol.club.v1.ClubBan)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional .bgs.protocol.club.v1.MemberId id = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_id()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_battle_tag;
        break;
      }

      // optional string battle_tag = 2;
      case 2: {
        if (tag == 18) {
         parse_battle_tag:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_battle_tag()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->battle_tag().data(), this->battle_tag().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "battle_tag");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(26)) goto parse_creator;
        break;
      }

      // optional .bgs.protocol.club.v1.MemberDescription creator = 3;
      case 3: {
        if (tag == 26) {
         parse_creator:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_creator()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(34)) goto parse_attribute;
        break;
      }

      // repeated .bgs.protocol.v2.Attribute attribute = 4;
      case 4: {
        if (tag == 34) {
         parse_attribute:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_attribute()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(34)) goto parse_attribute;
        if (input->ExpectTag(42)) goto parse_reason;
        break;
      }

      // optional string reason = 5;
      case 5: {
        if (tag == 42) {
         parse_reason:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_reason()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->reason().data(), this->reason().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "reason");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(48)) goto parse_creation_time;
        break;
      }

      // optional uint64 creation_time = 6;
      case 6: {
        if (tag == 48) {
         parse_creation_time:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, &creation_time_)));
          set_has_creation_time();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:bgs.protocol.club.v1.ClubBan)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:bgs.protocol.club.v1.ClubBan)
  return false;
#undef DO_
}

void ClubBan::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:bgs.protocol.club.v1.ClubBan)
  // optional .bgs.protocol.club.v1.MemberId id = 1;
  if (has_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->id(), output);
  }

  // optional string battle_tag = 2;
  if (has_battle_tag()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->battle_tag().data(), this->battle_tag().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "battle_tag");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->battle_tag(), output);
  }

  // optional .bgs.protocol.club.v1.MemberDescription creator = 3;
  if (has_creator()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      3, this->creator(), output);
  }

  // repeated .bgs.protocol.v2.Attribute attribute = 4;
  for (int i = 0; i < this->attribute_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      4, this->attribute(i), output);
  }

  // optional string reason = 5;
  if (has_reason()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->reason().data(), this->reason().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "reason");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      5, this->reason(), output);
  }

  // optional uint64 creation_time = 6;
  if (has_creation_time()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(6, this->creation_time(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:bgs.protocol.club.v1.ClubBan)
}

::google::protobuf::uint8* ClubBan::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:bgs.protocol.club.v1.ClubBan)
  // optional .bgs.protocol.club.v1.MemberId id = 1;
  if (has_id()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->id(), target);
  }

  // optional string battle_tag = 2;
  if (has_battle_tag()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->battle_tag().data(), this->battle_tag().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "battle_tag");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->battle_tag(), target);
  }

  // optional .bgs.protocol.club.v1.MemberDescription creator = 3;
  if (has_creator()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        3, this->creator(), target);
  }

  // repeated .bgs.protocol.v2.Attribute attribute = 4;
  for (int i = 0; i < this->attribute_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        4, this->attribute(i), target);
  }

  // optional string reason = 5;
  if (has_reason()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->reason().data(), this->reason().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "reason");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        5, this->reason(), target);
  }

  // optional uint64 creation_time = 6;
  if (has_creation_time()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(6, this->creation_time(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:bgs.protocol.club.v1.ClubBan)
  return target;
}

int ClubBan::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional .bgs.protocol.club.v1.MemberId id = 1;
    if (has_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->id());
    }

    // optional string battle_tag = 2;
    if (has_battle_tag()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->battle_tag());
    }

    // optional .bgs.protocol.club.v1.MemberDescription creator = 3;
    if (has_creator()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->creator());
    }

    // optional string reason = 5;
    if (has_reason()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->reason());
    }

    // optional uint64 creation_time = 6;
    if (has_creation_time()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt64Size(
          this->creation_time());
    }

  }
  // repeated .bgs.protocol.v2.Attribute attribute = 4;
  total_size += 1 * this->attribute_size();
  for (int i = 0; i < this->attribute_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->attribute(i));
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

void ClubBan::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ClubBan* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ClubBan*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ClubBan::MergeFrom(const ClubBan& from) {
  GOOGLE_CHECK_NE(&from, this);
  attribute_.MergeFrom(from.attribute_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_id()) {
      mutable_id()->::bgs::protocol::club::v1::MemberId::MergeFrom(from.id());
    }
    if (from.has_battle_tag()) {
      set_battle_tag(from.battle_tag());
    }
    if (from.has_creator()) {
      mutable_creator()->::bgs::protocol::club::v1::MemberDescription::MergeFrom(from.creator());
    }
    if (from.has_reason()) {
      set_reason(from.reason());
    }
    if (from.has_creation_time()) {
      set_creation_time(from.creation_time());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ClubBan::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ClubBan::CopyFrom(const ClubBan& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ClubBan::IsInitialized() const {
  if (has_id()) {
    if (!this->id().IsInitialized()) return false;
  }
  if (has_creator()) {
    if (!this->creator().IsInitialized()) return false;
  }
  return true;
}

void ClubBan::Swap(ClubBan* other) {
  if (other != this) {
    std::swap(id_, other->id_);
    std::swap(battle_tag_, other->battle_tag_);
    std::swap(creator_, other->creator_);
    attribute_.Swap(&other->attribute_);
    std::swap(reason_, other->reason_);
    std::swap(creation_time_, other->creation_time_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ClubBan::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ClubBan_descriptor_;
  metadata.reflection = ClubBan_reflection_;
  return metadata;
}

// @@protoc_insertion_point(namespace_scope)

}  // namespace v1
}  // namespace club
}  // namespace protocol
}  // namespace bgs

// @@protoc_insertion_point(global_scope)
