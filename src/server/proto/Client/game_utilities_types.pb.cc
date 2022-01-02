// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: game_utilities_types.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "game_utilities_types.pb.h"

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
namespace game_utilities {
namespace v1 {

namespace {

const ::google::protobuf::Descriptor* PlayerVariables_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  PlayerVariables_reflection_ = NULL;
const ::google::protobuf::Descriptor* ClientInfo_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ClientInfo_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_game_5futilities_5ftypes_2eproto() {
  protobuf_AddDesc_game_5futilities_5ftypes_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "game_utilities_types.proto");
  GOOGLE_CHECK(file != NULL);
  PlayerVariables_descriptor_ = file->message_type(0);
  static const int PlayerVariables_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PlayerVariables, identity_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PlayerVariables, rating_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PlayerVariables, attribute_),
  };
  PlayerVariables_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      PlayerVariables_descriptor_,
      PlayerVariables::default_instance_,
      PlayerVariables_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PlayerVariables, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PlayerVariables, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(PlayerVariables));
  ClientInfo_descriptor_ = file->message_type(1);
  static const int ClientInfo_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClientInfo, client_address_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClientInfo, privileged_network_),
  };
  ClientInfo_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ClientInfo_descriptor_,
      ClientInfo::default_instance_,
      ClientInfo_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClientInfo, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClientInfo, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ClientInfo));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_game_5futilities_5ftypes_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    PlayerVariables_descriptor_, &PlayerVariables::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ClientInfo_descriptor_, &ClientInfo::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_game_5futilities_5ftypes_2eproto() {
  delete PlayerVariables::default_instance_;
  delete PlayerVariables_reflection_;
  delete ClientInfo::default_instance_;
  delete ClientInfo_reflection_;
}

void protobuf_AddDesc_game_5futilities_5ftypes_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::bgs::protocol::protobuf_AddDesc_attribute_5ftypes_2eproto();
  ::bgs::protocol::protobuf_AddDesc_entity_5ftypes_2eproto();
  ::bgs::protocol::protobuf_AddDesc_rpc_5ftypes_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\032game_utilities_types.proto\022\036bgs.protoc"
    "ol.game_utilities.v1\032\025attribute_types.pr"
    "oto\032\022entity_types.proto\032\017rpc_types.proto"
    "\"w\n\017PlayerVariables\022(\n\010identity\030\001 \002(\0132\026."
    "bgs.protocol.Identity\022\016\n\006rating\030\002 \001(\001\022*\n"
    "\tattribute\030\003 \003(\0132\027.bgs.protocol.Attribut"
    "e\"@\n\nClientInfo\022\026\n\016client_address\030\001 \001(\t\022"
    "\032\n\022privileged_network\030\002 \001(\010B<\n\037bnet.prot"
    "ocol.game_utilities.v1B\027GameUtilitiesTyp"
    "esProtoH\001", 369);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "game_utilities_types.proto", &protobuf_RegisterTypes);
  PlayerVariables::default_instance_ = new PlayerVariables();
  ClientInfo::default_instance_ = new ClientInfo();
  PlayerVariables::default_instance_->InitAsDefaultInstance();
  ClientInfo::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_game_5futilities_5ftypes_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_game_5futilities_5ftypes_2eproto {
  StaticDescriptorInitializer_game_5futilities_5ftypes_2eproto() {
    protobuf_AddDesc_game_5futilities_5ftypes_2eproto();
  }
} static_descriptor_initializer_game_5futilities_5ftypes_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int PlayerVariables::kIdentityFieldNumber;
const int PlayerVariables::kRatingFieldNumber;
const int PlayerVariables::kAttributeFieldNumber;
#endif  // !_MSC_VER

PlayerVariables::PlayerVariables()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:bgs.protocol.game_utilities.v1.PlayerVariables)
}

void PlayerVariables::InitAsDefaultInstance() {
  identity_ = const_cast< ::bgs::protocol::Identity*>(&::bgs::protocol::Identity::default_instance());
}

PlayerVariables::PlayerVariables(const PlayerVariables& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:bgs.protocol.game_utilities.v1.PlayerVariables)
}

void PlayerVariables::SharedCtor() {
  _cached_size_ = 0;
  identity_ = NULL;
  rating_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

PlayerVariables::~PlayerVariables() {
  // @@protoc_insertion_point(destructor:bgs.protocol.game_utilities.v1.PlayerVariables)
  SharedDtor();
}

void PlayerVariables::SharedDtor() {
  if (this != default_instance_) {
    delete identity_;
  }
}

void PlayerVariables::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* PlayerVariables::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return PlayerVariables_descriptor_;
}

const PlayerVariables& PlayerVariables::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_game_5futilities_5ftypes_2eproto();
  return *default_instance_;
}

PlayerVariables* PlayerVariables::default_instance_ = NULL;

PlayerVariables* PlayerVariables::New() const {
  return new PlayerVariables;
}

void PlayerVariables::Clear() {
  if (_has_bits_[0 / 32] & 3) {
    if (has_identity()) {
      if (identity_ != NULL) identity_->::bgs::protocol::Identity::Clear();
    }
    rating_ = 0;
  }
  attribute_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool PlayerVariables::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:bgs.protocol.game_utilities.v1.PlayerVariables)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required .bgs.protocol.Identity identity = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_identity()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(17)) goto parse_rating;
        break;
      }

      // optional double rating = 2;
      case 2: {
        if (tag == 17) {
         parse_rating:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   double, ::google::protobuf::internal::WireFormatLite::TYPE_DOUBLE>(
                 input, &rating_)));
          set_has_rating();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(26)) goto parse_attribute;
        break;
      }

      // repeated .bgs.protocol.Attribute attribute = 3;
      case 3: {
        if (tag == 26) {
         parse_attribute:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_attribute()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(26)) goto parse_attribute;
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
  // @@protoc_insertion_point(parse_success:bgs.protocol.game_utilities.v1.PlayerVariables)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:bgs.protocol.game_utilities.v1.PlayerVariables)
  return false;
#undef DO_
}

void PlayerVariables::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:bgs.protocol.game_utilities.v1.PlayerVariables)
  // required .bgs.protocol.Identity identity = 1;
  if (has_identity()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->identity(), output);
  }

  // optional double rating = 2;
  if (has_rating()) {
    ::google::protobuf::internal::WireFormatLite::WriteDouble(2, this->rating(), output);
  }

  // repeated .bgs.protocol.Attribute attribute = 3;
  for (int i = 0; i < this->attribute_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      3, this->attribute(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:bgs.protocol.game_utilities.v1.PlayerVariables)
}

::google::protobuf::uint8* PlayerVariables::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:bgs.protocol.game_utilities.v1.PlayerVariables)
  // required .bgs.protocol.Identity identity = 1;
  if (has_identity()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->identity(), target);
  }

  // optional double rating = 2;
  if (has_rating()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteDoubleToArray(2, this->rating(), target);
  }

  // repeated .bgs.protocol.Attribute attribute = 3;
  for (int i = 0; i < this->attribute_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        3, this->attribute(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:bgs.protocol.game_utilities.v1.PlayerVariables)
  return target;
}

int PlayerVariables::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required .bgs.protocol.Identity identity = 1;
    if (has_identity()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->identity());
    }

    // optional double rating = 2;
    if (has_rating()) {
      total_size += 1 + 8;
    }

  }
  // repeated .bgs.protocol.Attribute attribute = 3;
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

void PlayerVariables::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const PlayerVariables* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const PlayerVariables*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void PlayerVariables::MergeFrom(const PlayerVariables& from) {
  GOOGLE_CHECK_NE(&from, this);
  attribute_.MergeFrom(from.attribute_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_identity()) {
      mutable_identity()->::bgs::protocol::Identity::MergeFrom(from.identity());
    }
    if (from.has_rating()) {
      set_rating(from.rating());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void PlayerVariables::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void PlayerVariables::CopyFrom(const PlayerVariables& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool PlayerVariables::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  if (has_identity()) {
    if (!this->identity().IsInitialized()) return false;
  }
  if (!::google::protobuf::internal::AllAreInitialized(this->attribute())) return false;
  return true;
}

void PlayerVariables::Swap(PlayerVariables* other) {
  if (other != this) {
    std::swap(identity_, other->identity_);
    std::swap(rating_, other->rating_);
    attribute_.Swap(&other->attribute_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata PlayerVariables::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = PlayerVariables_descriptor_;
  metadata.reflection = PlayerVariables_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int ClientInfo::kClientAddressFieldNumber;
const int ClientInfo::kPrivilegedNetworkFieldNumber;
#endif  // !_MSC_VER

ClientInfo::ClientInfo()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:bgs.protocol.game_utilities.v1.ClientInfo)
}

void ClientInfo::InitAsDefaultInstance() {
}

ClientInfo::ClientInfo(const ClientInfo& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:bgs.protocol.game_utilities.v1.ClientInfo)
}

void ClientInfo::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  client_address_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  privileged_network_ = false;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ClientInfo::~ClientInfo() {
  // @@protoc_insertion_point(destructor:bgs.protocol.game_utilities.v1.ClientInfo)
  SharedDtor();
}

void ClientInfo::SharedDtor() {
  if (client_address_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete client_address_;
  }
  if (this != default_instance_) {
  }
}

void ClientInfo::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ClientInfo::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ClientInfo_descriptor_;
}

const ClientInfo& ClientInfo::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_game_5futilities_5ftypes_2eproto();
  return *default_instance_;
}

ClientInfo* ClientInfo::default_instance_ = NULL;

ClientInfo* ClientInfo::New() const {
  return new ClientInfo;
}

void ClientInfo::Clear() {
  if (_has_bits_[0 / 32] & 3) {
    if (has_client_address()) {
      if (client_address_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        client_address_->clear();
      }
    }
    privileged_network_ = false;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ClientInfo::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:bgs.protocol.game_utilities.v1.ClientInfo)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional string client_address = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_client_address()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->client_address().data(), this->client_address().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "client_address");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_privileged_network;
        break;
      }

      // optional bool privileged_network = 2;
      case 2: {
        if (tag == 16) {
         parse_privileged_network:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &privileged_network_)));
          set_has_privileged_network();
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
  // @@protoc_insertion_point(parse_success:bgs.protocol.game_utilities.v1.ClientInfo)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:bgs.protocol.game_utilities.v1.ClientInfo)
  return false;
#undef DO_
}

void ClientInfo::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:bgs.protocol.game_utilities.v1.ClientInfo)
  // optional string client_address = 1;
  if (has_client_address()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->client_address().data(), this->client_address().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "client_address");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->client_address(), output);
  }

  // optional bool privileged_network = 2;
  if (has_privileged_network()) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(2, this->privileged_network(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:bgs.protocol.game_utilities.v1.ClientInfo)
}

::google::protobuf::uint8* ClientInfo::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:bgs.protocol.game_utilities.v1.ClientInfo)
  // optional string client_address = 1;
  if (has_client_address()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->client_address().data(), this->client_address().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "client_address");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->client_address(), target);
  }

  // optional bool privileged_network = 2;
  if (has_privileged_network()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(2, this->privileged_network(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:bgs.protocol.game_utilities.v1.ClientInfo)
  return target;
}

int ClientInfo::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional string client_address = 1;
    if (has_client_address()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->client_address());
    }

    // optional bool privileged_network = 2;
    if (has_privileged_network()) {
      total_size += 1 + 1;
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

void ClientInfo::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ClientInfo* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ClientInfo*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ClientInfo::MergeFrom(const ClientInfo& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_client_address()) {
      set_client_address(from.client_address());
    }
    if (from.has_privileged_network()) {
      set_privileged_network(from.privileged_network());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ClientInfo::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ClientInfo::CopyFrom(const ClientInfo& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ClientInfo::IsInitialized() const {

  return true;
}

void ClientInfo::Swap(ClientInfo* other) {
  if (other != this) {
    std::swap(client_address_, other->client_address_);
    std::swap(privileged_network_, other->privileged_network_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ClientInfo::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ClientInfo_descriptor_;
  metadata.reflection = ClientInfo_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace v1
}  // namespace game_utilities
}  // namespace protocol
}  // namespace bgs

// @@protoc_insertion_point(global_scope)
