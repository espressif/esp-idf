/** @file
 *  @brief Generic Attribute Profile handling.
 */

/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_GATT_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_GATT_H_

/**
 * @brief Generic Attribute Profile (GATT)
 * @details The GATT layer manages the service database by providing APIs for
 *          service registration and attribute declaration. For more
 *          information, see @ref bt_gatt_client and @ref bt_gatt_server.
 * @defgroup bt_gatt Generic Attribute Profile (GATT)
 * @ingroup bluetooth
 * @{
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <sys/types.h>

#include <zephyr/sys/slist.h>
#include <zephyr/sys/util.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/att.h>
#include <zephyr/sys/iterable_sections.h>

#ifdef __cplusplus
extern "C" {
#endif

/** GATT attribute permission bit field values */
enum bt_gatt_perm {
    /** No operations supported, e.g. for notify-only */
    BT_GATT_PERM_NONE = 0,

    /** Attribute read permission. */
    BT_GATT_PERM_READ = BIT(0),

    /** Attribute write permission. */
    BT_GATT_PERM_WRITE = BIT(1),

    /** @brief Attribute read permission with encryption.
     *
     *  If set, requires encryption for read access.
     */
    BT_GATT_PERM_READ_ENCRYPT = BIT(2),

    /** @brief Attribute write permission with encryption.
     *
     *  If set, requires encryption for write access.
     */
    BT_GATT_PERM_WRITE_ENCRYPT = BIT(3),

    /** @brief Attribute read permission with authentication.
     *
     *  If set, requires encryption using authenticated link-key for read
     *  access.
     */
    BT_GATT_PERM_READ_AUTHEN = BIT(4),

    /** @brief Attribute write permission with authentication.
     *
     *  If set, requires encryption using authenticated link-key for write
     *  access.
     */
    BT_GATT_PERM_WRITE_AUTHEN = BIT(5),

    /** @brief Attribute prepare write permission.
     *
     *  If set, allows prepare writes with use of ``BT_GATT_WRITE_FLAG_PREPARE``
     *  passed to write callback.
     */
    BT_GATT_PERM_PREPARE_WRITE = BIT(6),

    /** @brief Attribute read permission with LE Secure Connection encryption.
     *
     *  If set, requires that LE Secure Connections is used for read access.
     */
    BT_GATT_PERM_READ_LESC = BIT(7),

    /** @brief Attribute write permission with LE Secure Connection encryption.
     *
     *  If set, requires that LE Secure Connections is used for write access.
     */
    BT_GATT_PERM_WRITE_LESC = BIT(8),
};

/**
 *  @brief Construct error return value for attribute read and write callbacks.
 *
 *  @param _att_err ATT error code
 *
 *  @return Appropriate error code for the attribute callbacks.
 */
#define BT_GATT_ERR(_att_err) (-(_att_err))

/** GATT attribute write flags */
enum bt_gatt_attr_write_flag {
    /** @brief Attribute prepare write flag
     *
     * If set, write callback should only check if the device is
     * authorized but no data shall be written.
     */
    BT_GATT_WRITE_FLAG_PREPARE = BIT(0),

    /** @brief Attribute write command flag
     *
     * If set, indicates that write operation is a command (Write without
     * response) which doesn't generate any response.
     */
    BT_GATT_WRITE_FLAG_CMD = BIT(1),

    /** @brief Attribute write execute flag
     *
     * If set, indicates that write operation is a execute, which indicates
     * the end of a long write, and will come after 1 or more
     * @ref BT_GATT_WRITE_FLAG_PREPARE.
     */
    BT_GATT_WRITE_FLAG_EXECUTE = BIT(2),
};

/* Forward declaration of GATT Attribute structure */
struct bt_gatt_attr;

/** @typedef bt_gatt_attr_read_func_t
 *  @brief Attribute read callback
 *
 *  This is the type of the bt_gatt_attr.read() method.
 *
 *  This function may safely assume the Attribute Permissions
 *  are satisfied for this read. Callers are responsible for
 *  this.
 *
 *  Callers may set @p conn to emulate a GATT client read, or
 *  leave it NULL for local reads.
 *
 *  @note GATT server relies on this method to handle read
 *  operations from remote GATT clients. But this method is not
 *  reserved for the GATT server. E.g. You can lookup attributes
 *  in the local ATT database and invoke this method.
 *
 *  @note The GATT server propagates the return value from this
 *  method back to the remote client.
 *
 *  @param conn   The connection that is requesting to read.
 *                NULL if local.
 *  @param attr   The attribute that's being read
 *  @param buf    Buffer to place the read result in
 *  @param len    Length of data to read
 *  @param offset Offset to start reading from
 *
 *  @return Number of bytes read, or in case of an error
 *          ``BT_GATT_ERR()`` with a specific ``BT_ATT_ERR_*`` error code.
 */
typedef ssize_t (*bt_gatt_attr_read_func_t)(struct bt_conn *conn,
                                            const struct bt_gatt_attr *attr,
                                            void *buf, uint16_t len,
                                            uint16_t offset);

/** @typedef bt_gatt_attr_write_func_t
 *  @brief Attribute Value write implementation
 *
 *  This is the type of the bt_gatt_attr.write() method.
 *
 *  This function may safely assume the Attribute Permissions
 *  are satisfied for this write. Callers are responsible for
 *  this.
 *
 *  Callers may set @p conn to emulate a GATT client write, or
 *  leave it NULL for local writes.
 *
 *  If @p flags contains @ref BT_GATT_WRITE_FLAG_PREPARE, then
 *  the method shall not perform a write, but instead only check
 *  if the write is authorized and return an error code if not.
 *
 *  Attribute Value write implementations can and often do have
 *  side effects besides potentially storing the value. E.g.
 *  togging an LED.
 *
 *  @note GATT server relies on this method to handle write
 *  operations from remote GATT clients. But this method is not
 *  reserved for the GATT server. E.g. You can lookup attributes
 *  in the local ATT database and invoke this method.
 *
 *  @note The GATT server propagates the return value from this
 *  method back to the remote client.
 *
 *  @param conn   The connection that is requesting to write
 *  @param attr   The attribute that's being written
 *  @param buf    Buffer with the data to write
 *  @param len    Number of bytes in the buffer
 *  @param offset Offset to start writing from
 *  @param flags  Flags of type @ref bt_gatt_attr_write_flag
 *
 *  @return Number of bytes written, or in case of an error
 *          ``BT_GATT_ERR()`` with a specific ``BT_ATT_ERR_*`` error code.
 */
typedef ssize_t (*bt_gatt_attr_write_func_t)(struct bt_conn *conn,
                                             const struct bt_gatt_attr *attr,
                                             const void *buf, uint16_t len,
                                             uint16_t offset, uint8_t flags);

/** @brief GATT Attribute
 *
 *  This type primarily represents an ATT Attribute that may be
 *  an entry in the local ATT database. The objects of this type
 *  must be part of an array that forms a GATT service.
 *
 *  While the formed GATT service is registered with the local
 *  GATT server, pointers to this type can typically be given to
 *  GATT server APIs, like bt_gatt_notify().
 */
struct bt_gatt_attr {
    /** @brief Attribute Type
     *
     *  The Attribute Type is a UUID which determines the interface that can
     *  be expected from the read() and write() methods and
     *  the possible permission configurations.
     *
     *  E.g. Attribute of type @ref BT_UUID_GATT_CPF will act as a
     *  GATT Characteristic Presentation Format descriptor as
     *  specified in Core Specification 3.G.3.3.3.5.
     *
     *  You can define a new Attribute Type for your application specific
     *  use by generating a new UUID for it.
     */
    const struct bt_uuid *uuid;

    /** @brief Attribute Value read method
     *
     *  Readable Attributes must implement this method.
     *
     *  Must be NULL if the attribute is not readable.
     *
     *  The behavior of this method is determined by the Attribute
     *  Type.
     *
     *  See @ref bt_gatt_attr_read_func_t.
     */
    bt_gatt_attr_read_func_t read;

    /** @brief Attribute Value write method
     *
     *  Writeable Attributes must implement this method.
     *
     *  Must be NULL if the attribute is not writable.
     *
     *  The behavior of this method is determined by the Attribute
     *  Type.
     *
     *  See @ref bt_gatt_attr_write_func_t.
     */
    bt_gatt_attr_write_func_t write;

    /** @brief Private data for read() and write() implementation
     *
     *  The meaning of this field varies and is not specified here.
     *
     *  @note Attributes may have the same Attribute Type but have
     *  different implementations, with incompatible user data.
     *  Attribute Type alone must not be used to infer the type of
     *  the user data.
     *
     *  @sa bt_gatt_discover_func_t about this field.
     */
    void *user_data;

    /** @brief Attribute Handle
     *
     *  The Attribute Handle is an index corresponding to a specific
     *  Attribute in the ATT database.
     *
     *  @note Use bt_gatt_attr_get_handle() for attributes in the
     *  local ATT database.
     */
    uint16_t handle;

    /** @brief Attribute Permissions
     *
     *  Bit field of @ref bt_gatt_perm.
     *
     *  The permissions are security requirements that must be
     *  satisfied before calling read() or write().
     */
    uint16_t perm: 15;

    /** @cond INTERNAL_HIDDEN
     *  Indicates if the attribute handle was assigned automatically.
     *
     *  This flag is set to 1 if the attribute handle was assigned by the stack,
     *  and 0 if it was manually set by the application.
     *
     *  @note Applications must not modify this field.
     */
    bool _auto_assigned_handle: 1;
    /** @endcond */
};

/** @brief Static GATT Service structure
 *
 *  Allows the user the declare static GATT Services with the aim of reducing the
 *  used RAM. The @ref BT_GATT_SERVICE_DEFINE macro can be used to statically
 *  define and register a service.
 */
struct bt_gatt_service_static {
    /** Service Attributes */
    struct bt_gatt_attr *attrs;
    /** Service Attribute count */
    size_t attr_count;
};

/** @brief GATT Service structure
 *
 *  This structure is used to define GATT services which can be registered and
 *  unregistered at runtime. See @ref bt_gatt_service_register for when services
 *  should be registered.
 */
struct bt_gatt_service {
    /** Service Attributes */
    struct bt_gatt_attr *attrs;
    /** Service Attribute count */
    size_t attr_count;
    /** @cond INTERNAL_HIDDEN
     *  Field used for list handling.
     */
    sys_snode_t node;
    /** @endcond */
};

/** @brief Service Attribute Value.
 *
 *  This is the data described by the Attribute Type and indexed by the
 *  Attribute Handle in the database.
 */
struct bt_gatt_service_val {
    /** Service UUID. */
    const struct bt_uuid *uuid;
    /** Handle of the last Attribute within the Service. */
    uint16_t end_handle;
};

/** @brief Include Attribute Value.
 *
 *  This structure represents an included service attribute in the GATT
 *  server. An included service is a service that is referenced within another
 *  service, allowing for the reuse of common service definitions.
 */
struct bt_gatt_include {
    /** Service UUID. */
    const struct bt_uuid *uuid;
    /** Handle of the first attribute within the included service. */
    uint16_t start_handle;
    /** Handle of the last attribute within the included service. */
    uint16_t end_handle;
};

/** Characteristic Properties Bit field values */

/**
 *  @brief Characteristic broadcast property.
 *
 *  If set, permits broadcasts of the Characteristic Value using Server
 *  Characteristic Configuration Descriptor.
 */
#define BT_GATT_CHRC_BROADCAST          0x01
/**
 *  @brief Characteristic read property.
 *
 *  If set, permits reads of the Characteristic Value.
 */
#define BT_GATT_CHRC_READ           0x02
/**
 *  @brief Characteristic write without response property.
 *
 *  If set, permits write of the Characteristic Value without response.
 */
#define BT_GATT_CHRC_WRITE_WITHOUT_RESP     0x04
/**
 *  @brief Characteristic write with response property.
 *
 *  If set, permits write of the Characteristic Value with response.
 */
#define BT_GATT_CHRC_WRITE          0x08
/**
 *  @brief Characteristic notify property.
 *
 *  If set, permits notifications of a Characteristic Value without
 *  acknowledgment.
 */
#define BT_GATT_CHRC_NOTIFY         0x10
/**
 *  @brief Characteristic indicate property.
 *
 * If set, permits indications of a Characteristic Value with acknowledgment.
 */
#define BT_GATT_CHRC_INDICATE           0x20
/**
 *  @brief Characteristic Authenticated Signed Writes property.
 *
 *  If set, permits signed writes to the Characteristic Value.
 */
#define BT_GATT_CHRC_AUTH           0x40
/**
 *  @brief Characteristic Extended Properties property.
 *
 * If set, additional characteristic properties are defined in the
 * Characteristic Extended Properties Descriptor.
 */
#define BT_GATT_CHRC_EXT_PROP           0x80

/** @brief Attribute Value of a Characteristic Declaration.
 *
 *  This is the data associated with the characteristic, and can be read from or
 *  written to by a GATT client depending on the characteristic properties.
 */
struct bt_gatt_chrc {
    /** Characteristic UUID. */
    const struct bt_uuid *uuid;
    /** Characteristic Value handle. */
    uint16_t value_handle;
    /** Characteristic properties, a bitmap of ``BT_GATT_CHRC_*`` macros. */
    uint8_t properties;
};

/** Characteristic Extended Properties Bit field values */
#define BT_GATT_CEP_RELIABLE_WRITE      0x0001
#define BT_GATT_CEP_WRITABLE_AUX        0x0002

/** @brief Characteristic Extended Properties Attribute Value.
 *
 *  Used in the discovery of standard characteristic descriptor values. Shall
 *  exist if the @ref BT_GATT_CHRC_EXT_PROP bit is set in the characteristic
 *  properties. Can be used with the @ref BT_GATT_CEP macro to declare the CEP
 *  descriptor.
 */
struct bt_gatt_cep {
    /** Characteristic Extended properties, a bitmap of ``BT_GATT_CEP_*`` macros. */
    uint16_t properties;
};

/** Client Characteristic Configuration Values */

/**
 *  @brief Client Characteristic Configuration Notification.
 *
 *  If set, changes to Characteristic Value shall be notified.
 */
#define BT_GATT_CCC_NOTIFY          0x0001
/**
 *  @brief Client Characteristic Configuration Indication.
 *
 *  If set, changes to Characteristic Value shall be indicated.
 */
#define BT_GATT_CCC_INDICATE            0x0002

/** @brief Client Characteristic Configuration Attribute Value
 *
 *  Used in the discovery of standard characteristic descriptor values.
 */
struct bt_gatt_ccc {
    /** Client Characteristic Configuration flags, a bitmap of ``BT_GATT_CCC_*`` macros. */
    uint16_t flags;
};

/** Server Characteristic Configuration Values */

/**
 *  @brief Server Characteristic Configuration Broadcast
 *
 *  If set, the characteristic value shall be broadcast in the advertising data
 *  when the server is advertising.
 */
#define BT_GATT_SCC_BROADCAST                   0x0001

/** @brief Server Characteristic Configuration Attribute Value
 *
 *  Used in the discovery of standard characteristic descriptor values.
 */
struct bt_gatt_scc {
    /** Server Characteristic Configuration flags, a bitmap of ``BT_GATT_SCC_*`` macros. */
    uint16_t flags;
};

/** @brief GATT Characteristic Presentation Format Attribute Value.
 *
 *  Used in the discovery of standard characteristic descriptor values. Can be
 *  used with the @ref BT_GATT_CPF macro to declare the CPF descriptor.
 */
struct bt_gatt_cpf {
    /** @brief Format of the value of the characteristic.
     *
     *  The format types can be found in section 2.4.1 of the Bluetooth SIG
     *  Assigned Numbers document.
     */
    uint8_t format;
    /** @brief Exponent field for value formatting.
     *
     *  Only used on integer format types.
     *  actual value = Characteristic Value x 10^Exponent
     */
    int8_t exponent;
    /** @brief UUID of the unit of the characteristic.
     *
     *  The units can be found in section 3.5 of the Bluetooth SIG Assigned
     *  Numbers document.
     */
    uint16_t unit;
    /** @brief Name space of the description.
     *
     *  Used to identify the organization that is responsible for defining
     *  the enumerations for the description field. See section 2.4.2 of the
     *  Bluetooth SIG Assigned Numbers document.
     */
    uint8_t name_space;
    /** @brief Description of the characteristic as defined in a higher layer profile.
     *
     *  An enumerated value defined by the organization identified by the
     *  name_space field. See section 2.4.2.1 of the Bluetooth SIG Assigned
     *  Numbers document.
     */
    uint16_t description;
};

/**
 * @defgroup bt_gatt_server GATT Server APIs
 * @ingroup bt_gatt
 * @{
 */

/** @brief Register GATT service.
 *
 *  To register a GATT service, applications can make use of macros such as
 *  ``BT_GATT_PRIMARY_SERVICE``, ``BT_GATT_CHARACTERISTIC``,
 *  ``BT_GATT_DESCRIPTOR``, etc.
 *
 *  When using @kconfig{CONFIG_BT_SETTINGS} then all services that should have
 *  bond configuration loaded, i.e. CCC values, must be registered before
 *  calling @ref settings_load.
 *
 *  When using @kconfig{CONFIG_BT_GATT_CACHING} and @kconfig{CONFIG_BT_SETTINGS}
 *  then all services that should be included in the GATT Database Hash
 *  calculation should be added before calling @ref settings_load.
 *  All services registered after settings_load will trigger a new database hash
 *  calculation and a new hash stored.
 *
 *  There are two situations where this function can be called: either before
 *  `bt_init()` has been called, or after `settings_load()` has been called.
 *  Registering a service in the middle is not supported and will return an
 *  error.
 *
 *  @param svc Service containing the available attributes
 *
 *  @return 0 in case of success or negative value in case of error.
 *  @return -EAGAIN if ``bt_init()`` has been called but ``settings_load()`` hasn't yet.
 */
int bt_gatt_service_register(struct bt_gatt_service *svc);
int bt_gatt_service_register_safe(struct bt_gatt_service *svc);

/** @brief Unregister GATT service.
 *
 *  @param svc Service to be unregistered.
 *
 *  @return 0 in case of success or negative value in case of error.
 */
int bt_gatt_service_unregister(struct bt_gatt_service *svc);
int bt_gatt_service_unregister_safe(struct bt_gatt_service *svc);

/** @brief to be used as return values for @ref bt_gatt_attr_func_t and @ref bt_gatt_read_func_t
 *  type callbacks.
 */
enum bt_gatt_iter {
    BT_GATT_ITER_STOP = 0,
    BT_GATT_ITER_CONTINUE,
};

/** @typedef bt_gatt_attr_func_t
 *  @brief Attribute iterator callback.
 *
 *  @param attr Attribute found.
 *  @param handle Attribute handle found.
 *  @param user_data Data given.
 *
 *  @return ``BT_GATT_ITER_CONTINUE`` if should continue to the next attribute.
 *  @return ``BT_GATT_ITER_STOP`` to stop.
 */
typedef uint8_t (*bt_gatt_attr_func_t)(const struct bt_gatt_attr *attr,
                                       uint16_t handle,
                                       void *user_data);

/** @brief Attribute iterator by type.
 *
 *  Iterate attributes in the given range matching given UUID and/or data.
 *
 *  @param start_handle Start attribute handle.
 *  @param end_handle End attribute handle. Often set to start_handle + attr_count or
 *  BT_ATT_LAST_ATTRIBUTE_HANDLE.
 *  @param uuid UUID to match, passing NULL skips UUID matching.
 *  @param attr_data Attribute data to match, passing NULL skips data matching.
 *  @param num_matches Number matches, passing 0 makes it unlimited.
 *  @param func Callback function.
 *  @param user_data Data to pass to the callback.
 */
void bt_gatt_foreach_attr_type(uint16_t start_handle, uint16_t end_handle,
                               const struct bt_uuid *uuid,
                               const void *attr_data, uint16_t num_matches,
                               bt_gatt_attr_func_t func,
                               void *user_data);

/** @brief Attribute iterator.
 *
 *  Iterate attributes in the given range.
 *
 *  @param start_handle Starting attribute handle.
 *  @param end_handle Ending attribute handle.
 *  @param func Callback function.
 *  @param user_data Data to pass to the callback.
 */
static inline void bt_gatt_foreach_attr(uint16_t start_handle, uint16_t end_handle,
                                        bt_gatt_attr_func_t func,
                                        void *user_data)
{
    bt_gatt_foreach_attr_type(start_handle, end_handle, NULL, NULL, 0, func,
                              user_data);
}

/** @brief Iterate to the next attribute
 *
 *  Iterate to the next attribute following a given attribute.
 *
 *  @param attr Current Attribute.
 *
 *  @return The next attribute or NULL if it cannot be found.
 */
struct bt_gatt_attr *bt_gatt_attr_next(const struct bt_gatt_attr *attr);

/** @brief Find Attribute by UUID.
 *
 *  Find the attribute with the matching UUID.
 *  To limit the search to a service set the attr to the service attributes and
 *  the ``attr_count`` to the service attribute count .
 *
 *  @param attr        Pointer to an attribute that serves as the starting point
 *                     for the search of a match for the UUID.
 *                     Passing NULL will search the entire range.
 *  @param attr_count  The number of attributes from the starting point to
 *                     search for a match for the UUID.
 *                     Set to 0 to search until the end.
 *  @param uuid        UUID to match.
 */
struct bt_gatt_attr *bt_gatt_find_by_uuid(const struct bt_gatt_attr *attr,
                                          uint16_t attr_count,
                                          const struct bt_uuid *uuid);

/** @brief Get Attribute handle.
 *
 *  @param attr An attribute object currently registered in the
 *  local ATT server.
 *
 *  @return Handle of the corresponding attribute or zero if the attribute
 *          could not be found.
 */
uint16_t bt_gatt_attr_get_handle(const struct bt_gatt_attr *attr);

/** @brief Get the handle of the characteristic value descriptor.
 *
 * @param attr A Characteristic Attribute.
 *
 * @note The ``user_data`` of the attribute must be of type @ref bt_gatt_chrc and the ``uuid`` shall
 * be BT_UUID_GATT_CHRC.
 *
 * @return the handle of the corresponding Characteristic Value. The value will
 *         be zero (the invalid handle) if @p attr was not a characteristic
 *         attribute.
 */
uint16_t bt_gatt_attr_value_handle(const struct bt_gatt_attr *attr);

/** @brief Generic Read Attribute value helper.
 *
 *  Read attribute value from local database storing the result into buffer.
 *
 *  @param conn Connection object.
 *  @param attr Attribute to read.
 *  @param buf Buffer to store the value.
 *  @param buf_len Buffer length.
 *  @param offset Start offset.
 *  @param value Attribute value.
 *  @param value_len Length of the attribute value.
 *
 *  @return number of bytes read in case of success or negative values in
 *          case of error.
 */
ssize_t bt_gatt_attr_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          void *buf, uint16_t buf_len, uint16_t offset,
                          const void *value, uint16_t value_len);

/** @brief Read Service Attribute helper.
 *
 *  Read service attribute value from local database storing the result into
 *  buffer after encoding it.
 *  @note Only use this with attributes which ``user_data`` is a ``bt_uuid``.
 *
 *  @param conn Connection object.
 *  @param attr Attribute to read.
 *  @param buf Buffer to store the value read.
 *  @param len Buffer length.
 *  @param offset Start offset.
 *
 *  @return number of bytes read in case of success or negative values in
 *          case of error.
 */
ssize_t bt_gatt_attr_read_service(struct bt_conn *conn,
                                  const struct bt_gatt_attr *attr,
                                  void *buf, uint16_t len, uint16_t offset);

/**
 *  @brief Statically define and register a service.
 *
 *  Helper macro to statically define and register a service.
 *
 *  @param _name Service name.
 */
#define BT_GATT_SERVICE_DEFINE(_name, ...)              \
    struct bt_gatt_attr attr_##_name[] = { __VA_ARGS__ };   \
    STRUCT_SECTION_ITERABLE(bt_gatt_service_static, _name) =    \
                    BT_GATT_SERVICE(attr_##_name)

#define _BT_GATT_ATTRS_ARRAY_DEFINE(n, _instances, _attrs_def)  \
    static struct bt_gatt_attr attrs_##n[] = _attrs_def(_instances[n])

#define _BT_GATT_SERVICE_ARRAY_ITEM(_n, _) BT_GATT_SERVICE(attrs_##_n)

/**
 *  @brief Statically define service structure array.
 *
 *  Helper macro to statically define service structure array. Each element
 *  of the array is linked to the service attribute array which is also
 *  defined in this scope using ``_attrs_def`` macro.
 *
 *  @param _name         Name of service structure array.
 *  @param _instances    Array of instances to pass as user context to the
 *                       attribute callbacks.
 *  @param _instance_num Number of elements in instance array.
 *  @param _attrs_def    Macro provided by the user that defines attribute
 *                       array for the service. This macro should accept single
 *                       parameter which is the instance context.
 */
#define BT_GATT_SERVICE_INSTANCE_DEFINE(                 \
    _name, _instances, _instance_num, _attrs_def)            \
    BUILD_ASSERT(ARRAY_SIZE(_instances) == _instance_num,        \
        "The number of array elements does not match its size"); \
    LISTIFY(_instance_num, _BT_GATT_ATTRS_ARRAY_DEFINE, (;),     \
        _instances, _attrs_def);                 \
    static struct bt_gatt_service _name[] = {            \
        LISTIFY(_instance_num, _BT_GATT_SERVICE_ARRAY_ITEM, (,)) \
    }

/**
 *  @brief Service Structure Declaration Macro.
 *
 *  Helper macro to declare a service structure.
 *
 *  @param _attrs Service attributes.
 */
#define BT_GATT_SERVICE(_attrs)                     \
{                                   \
    .attrs = _attrs,                        \
    .attr_count = ARRAY_SIZE(_attrs),               \
}

/**
 *  @brief Primary Service Declaration Macro.
 *
 *  Helper macro to declare a primary service attribute.
 *
 *  @param _service Service attribute value.
 */
#define BT_GATT_PRIMARY_SERVICE(_service)               \
    BT_GATT_ATTRIBUTE(BT_UUID_GATT_PRIMARY, BT_GATT_PERM_READ,  \
             bt_gatt_attr_read_service, NULL, (void *)_service)

/**
 *  @brief Secondary Service Declaration Macro.
 *
 *  Helper macro to declare a secondary service attribute.
 *
 *  @note A secondary service is only intended to be included from a primary
 *  service or another secondary service or other higher layer specification.
 *
 *  @param _service Service attribute value.
 */
#define BT_GATT_SECONDARY_SERVICE(_service)             \
    BT_GATT_ATTRIBUTE(BT_UUID_GATT_SECONDARY, BT_GATT_PERM_READ,    \
             bt_gatt_attr_read_service, NULL, (void *)_service)

/** @brief Read Include Attribute helper.
 *
 *  Read include service attribute value from local database storing the result
 *  into buffer after encoding it.
 *  @note Only use this with attributes which user_data is a ``bt_gatt_include``.
 *  The function returns EINVAL if @p attr or @p attr->user_data is NULL.
 *
 *  @param conn Connection object.
 *  @param attr Attribute to read.
 *  @param buf Buffer to store the value read.
 *  @param len Buffer length.
 *  @param offset Start offset.
 *
 *  @return number of bytes read in case of success or negative values in
 *          case of error.
 */
ssize_t bt_gatt_attr_read_included(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr,
                                   void *buf, uint16_t len, uint16_t offset);

/**
 *  @brief Include Service Declaration Macro.
 *
 *  Helper macro to declare database internal include service attribute.
 *
 *  @param _service_incl the first service attribute of service to include
 */
#define BT_GATT_INCLUDE_SERVICE(_service_incl)              \
    BT_GATT_ATTRIBUTE(BT_UUID_GATT_INCLUDE, BT_GATT_PERM_READ,  \
              bt_gatt_attr_read_included, NULL, _service_incl)

/** @brief Read Characteristic Attribute helper.
 *
 *  Read characteristic attribute value from local database storing the result
 *  into buffer after encoding it.
 *  @note Only use this with attributes which ``user_data`` is a ``bt_gatt_chrc``.
 *
 *  @param conn Connection object.
 *  @param attr Attribute to read.
 *  @param buf Buffer to store the value read.
 *  @param len Buffer length.
 *  @param offset Start offset.
 *
 *  @return number of bytes read in case of success or negative values in
 *          case of error.
 */
ssize_t bt_gatt_attr_read_chrc(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr, void *buf,
                               uint16_t len, uint16_t offset);

/** @brief Gatt Characteristic Initialization Macro.
 *
 *  Helper macro used within the @ref BT_GATT_CHARACTERISTIC macro in the GATT attribute declaration
 *  to set the attribute user data.
 *
 *  @param _uuid Characteristic attribute uuid.
 *  @param _handle Characcteristic attribute handle at init.
 *  @param _props Characteristic attribute properties,
 *                a bitmap of ``BT_GATT_CHRC_*`` macros.
 */
#define BT_GATT_CHRC_INIT(_uuid, _handle, _props) \
{                                                 \
    .uuid = _uuid,                            \
    .value_handle = _handle,                  \
    .properties = _props,                     \
}

/**
 *  @brief Characteristic and Value Declaration Macro.
 *
 *  Helper macro to declare a characteristic attribute along with its
 *  attribute value.
 *
 *  @param _uuid Characteristic attribute uuid.
 *  @param _props Characteristic attribute properties,
 *                a bitmap of ``BT_GATT_CHRC_*`` macros.
 *  @param _perm Characteristic Attribute access permissions,
 *               a bitmap of @ref bt_gatt_perm values.
 *  @param _read Characteristic Attribute read callback
 *               (@ref bt_gatt_attr_read_func_t).
 *  @param _write Characteristic Attribute write callback
 *                (@ref bt_gatt_attr_write_func_t).
 *  @param _user_data Characteristic Attribute user data.
 */
#define BT_GATT_CHARACTERISTIC(_uuid, _props, _perm, _read, _write, _user_data) \
    BT_GATT_ATTRIBUTE(BT_UUID_GATT_CHRC, BT_GATT_PERM_READ,                 \
              bt_gatt_attr_read_chrc, NULL,                         \
              ((struct bt_gatt_chrc[]) {                            \
                BT_GATT_CHRC_INIT(_uuid, 0U, _props),           \
                           })),                         \
    BT_GATT_ATTRIBUTE(_uuid, _perm, _read, _write, _user_data)

#define BT_GATT_CCC_MAX (1)

/** @brief GATT CCC configuration entry.
 *
 *  bt_gatt_ccc_cfg is used within @ref bt_gatt_attr_read_ccc and @ref bt_gatt_attr_write_ccc to
 *  read and write the ccc configurations respectively.
 *
 */
struct bt_gatt_ccc_cfg {
    /** Local identity, BT_ID_DEFAULT in most cases. */
    uint8_t id;
    /** Remote peer address. */
    bt_addr_le_t peer;
    /** @brief Configuration value
     * Value used to enable or disable notifications or indications for a specific
     * characteristic.
     */
    uint16_t value;
};

/** Internal representation of CCC value */
struct bt_gatt_ccc_managed_user_data {
    /** Configuration for each connection */
    struct bt_gatt_ccc_cfg cfg[BT_GATT_CCC_MAX];

    /** Highest value of all connected peer's subscriptions */
    uint16_t value;

    /** @brief CCC attribute changed callback
     *
     *  @param attr   The attribute that's changed value
     *  @param value  New value
     */
    void (*cfg_changed)(const struct bt_gatt_attr *attr, uint16_t value);

    /** @brief CCC attribute write validation callback
     *
     *  @param conn   The connection that is requesting to write
     *  @param attr   The attribute that's being written
     *  @param value  CCC value to write
     *
     *  @return Number of bytes to write, or in case of an error
     *          BT_GATT_ERR() with a specific error code.
     */
    ssize_t (*cfg_write)(struct bt_conn *conn,
                         const struct bt_gatt_attr *attr, uint16_t value);

    /** @brief CCC attribute match handler
     *
     *  Indicate if it is OK to send a notification or indication
     *  to the subscriber.
     *
     *  @param conn   The connection that is being checked
     *  @param attr   The attribute that's being checked
     *
     *  @return true  if application has approved notification/indication,
     *          false if application does not approve.
     */
    bool (*cfg_match)(struct bt_conn *conn,
                      const struct bt_gatt_attr *attr);
};

/** @brief Read Client Characteristic Configuration Attribute helper.
 *
 *  Read CCC attribute value from local database storing the result into buffer
 *  after encoding it.
 *
 *  @param conn Connection object.
 *  @param attr Attribute to read.
 *  @param buf Buffer to store the value read.
 *  @param len Buffer length.
 *  @param offset Start offset.
 *
 *  @return number of bytes read in case of success or negative values in
 *          case of error.
 */
/** @cond INTERNAL_HIDDEN
 *  @note Only use this with attributes which user_data is a bt_gatt_ccc_managed_user_data.
 *        @ref bt_gatt_ccc_managed_user_data being the internal representation of CCC value.
 */
/** @endcond */
ssize_t bt_gatt_attr_read_ccc(struct bt_conn *conn,
                              const struct bt_gatt_attr *attr, void *buf,
                              uint16_t len, uint16_t offset);

/** @brief Write Client Characteristic Configuration Attribute helper.
 *
 *  Write value in the buffer into CCC attribute.
 *
 *  @param conn Connection object.
 *  @param attr Attribute to read.
 *  @param buf Buffer to store the value read.
 *  @param len Buffer length.
 *  @param offset Start offset.
 *  @param flags Write flags, see @ref bt_gatt_attr_write_flag.
 *
 *  @return number of bytes written in case of success or negative values in
 *          case of error.
 */
/** @cond INTERNAL_HIDDEN
 *  @note Only use this with attributes which user_data is a bt_gatt_ccc_managed_user_data.
 *        @ref bt_gatt_ccc_managed_user_data being the internal representation of CCC value.
 */
/** @endcond */
ssize_t bt_gatt_attr_write_ccc(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr, const void *buf,
                               uint16_t len, uint16_t offset, uint8_t flags);

/**
 *  @brief Initialize Client Characteristic Configuration Declaration Macro.
 *
 *  Helper macro to initialize a Managed CCC attribute value.
 *
 *  @param _changed Configuration changed callback.
 *  @param _write Configuration write callback.
 *  @param _match Configuration match callback.
 */
#define BT_GATT_CCC_MANAGED_USER_DATA_INIT(_changed, _write, _match) \
    {                                            \
        .cfg = {                             \
            [0 ... (BT_GATT_CCC_MAX - 1)] = {0}, \
        },                                   \
        .cfg_changed = _changed,             \
        .cfg_write = _write,                 \
        .cfg_match = _match,                 \
    }

/**
 *  @brief Managed Client Characteristic Configuration Declaration Macro.
 *
 *  Helper macro to declare a Managed CCC attribute.
 *
 *  @param _ccc A new @ref bt_gatt_ccc_managed_user_data object with the same lifetime
 *              as the results of the call to BT_GATT_CCC_MANAGED.
 *              See the documentation of @ref bt_gatt_ccc_managed_user_data on how
 *              to initialize it.
 *  @param _perm CCC access permissions,
 *               a bitmap of @ref bt_gatt_perm values.
 */
#define BT_GATT_CCC_MANAGED(_ccc, _perm) \
    BT_GATT_ATTRIBUTE(BT_UUID_GATT_CCC, _perm, \
                      bt_gatt_attr_read_ccc, bt_gatt_attr_write_ccc, \
                      _ccc)

/**
 *  @brief Client Characteristic Configuration Declaration Macro.
 *
 *  Helper macro to declare a CCC attribute.
 *
 *  @param _changed Configuration changed callback.
 *  @param _perm CCC access permissions,
 *               a bitmap of @ref bt_gatt_perm values.
 */
#define BT_GATT_CCC(_changed, _perm) \
    BT_GATT_CCC_MANAGED(((struct bt_gatt_ccc_managed_user_data[]){ \
                        BT_GATT_CCC_MANAGED_USER_DATA_INIT(_changed, NULL, NULL)}), \
                        _perm)

/** @brief Read Characteristic Extended Properties Attribute helper
 *
 *  Read CEP attribute value from local database storing the result into buffer
 *  after encoding it.
 *
 *  @note Only use this with attributes which user_data is a bt_gatt_cep.
 *
 *  @param conn Connection object
 *  @param attr Attribute to read
 *  @param buf Buffer to store the value read
 *  @param len Buffer length
 *  @param offset Start offset
 *
 *  @return number of bytes read in case of success or negative values in
 *          case of error.
 */
ssize_t bt_gatt_attr_read_cep(struct bt_conn *conn,
                              const struct bt_gatt_attr *attr, void *buf,
                              uint16_t len, uint16_t offset);

/**
 *  @brief Characteristic Extended Properties Declaration Macro.
 *
 *  Helper macro to declare a CEP attribute.
 *
 *  @param _value Pointer to a struct bt_gatt_cep.
 */
#define BT_GATT_CEP(_value)                     \
    BT_GATT_DESCRIPTOR(BT_UUID_GATT_CEP, BT_GATT_PERM_READ,     \
              bt_gatt_attr_read_cep, NULL, (void *)_value)

/** @brief Read Characteristic User Description Descriptor Attribute helper
 *
 *  Read CUD attribute value from local database storing the result into buffer
 *  after encoding it.
 *
 *  @note Only use this with attributes which user_data is a NULL-terminated C
 *        string.
 *
 *  @param conn Connection object
 *  @param attr Attribute to read
 *  @param buf Buffer to store the value read
 *  @param len Buffer length
 *  @param offset Start offset
 *
 *  @return number of bytes read in case of success or negative values in
 *          case of error.
 */
ssize_t bt_gatt_attr_read_cud(struct bt_conn *conn,
                              const struct bt_gatt_attr *attr, void *buf,
                              uint16_t len, uint16_t offset);

/**
 *  @brief Characteristic User Format Descriptor Declaration Macro.
 *
 *  Helper macro to declare a CUD attribute.
 *
 *  @param _value User description NULL-terminated C string.
 *  @param _perm Descriptor attribute access permissions,
 *               a bitmap of @ref bt_gatt_perm values.
 */
#define BT_GATT_CUD(_value, _perm)                  \
    BT_GATT_DESCRIPTOR(BT_UUID_GATT_CUD, _perm, bt_gatt_attr_read_cud, \
               NULL, (void *)_value)

/** @brief Read Characteristic Presentation format Descriptor Attribute helper
 *
 *  Read CPF attribute value from local database storing the result into buffer
 *  after encoding it.
 *
 *  @note Only use this with attributes which user_data is a @ref bt_gatt_cpf.
 *
 *  @param conn Connection object
 *  @param attr Attribute to read
 *  @param buf Buffer to store the value read
 *  @param len Buffer length
 *  @param offset Start offset
 *
 *  @return number of bytes read in case of success or negative values in
 *          case of error.
 */
ssize_t bt_gatt_attr_read_cpf(struct bt_conn *conn,
                              const struct bt_gatt_attr *attr, void *buf,
                              uint16_t len, uint16_t offset);

/**
 *  @brief Characteristic Presentation Format Descriptor Declaration Macro.
 *
 *  Helper macro to declare a CPF attribute.
 *
 *  @param _value Pointer to a struct bt_gatt_cpf.
 */
#define BT_GATT_CPF(_value)                     \
    BT_GATT_DESCRIPTOR(BT_UUID_GATT_CPF, BT_GATT_PERM_READ,     \
              bt_gatt_attr_read_cpf, NULL, (void *)_value)

/**
 *  @brief Descriptor Declaration Macro.
 *
 *  Helper macro to declare a descriptor attribute.
 *
 *  @param _uuid Descriptor attribute uuid.
 *  @param _perm Descriptor attribute access permissions,
 *               a bitmap of @ref bt_gatt_perm values.
 *  @param _read Descriptor attribute read callback
 *               (@ref bt_gatt_attr_read_func_t).
 *  @param _write Descriptor attribute write callback
 *                (@ref bt_gatt_attr_write_func_t).
 *  @param _user_data Descriptor attribute user data.
 */
#define BT_GATT_DESCRIPTOR(_uuid, _perm, _read, _write, _user_data) \
    BT_GATT_ATTRIBUTE(_uuid, _perm, _read, _write, _user_data)

/**
 *  @brief Attribute Declaration Macro.
 *
 *  Helper macro to declare an attribute.
 *
 *  @param _uuid Attribute uuid.
 *  @param _perm Attribute access permissions,
 *               a bitmap of @ref bt_gatt_perm values.
 *  @param _read Attribute read callback (@ref bt_gatt_attr_read_func_t).
 *  @param _write Attribute write callback (@ref bt_gatt_attr_write_func_t).
 *  @param _user_data Attribute user data.
 */
#define BT_GATT_ATTRIBUTE(_uuid, _perm, _read, _write, _user_data)  \
{                                   \
    .uuid = _uuid,                          \
    .read = _read,                          \
    .write = _write,                        \
    .user_data = _user_data,                    \
    .handle = 0,                            \
    .perm = _perm,                          \
}

/** @brief Notification complete result callback.
 *
 *  @param conn Connection object.
 *  @param user_data Data passed in by the user.
 */
typedef void (*bt_gatt_complete_func_t)(struct bt_conn *conn, void *user_data);

/** @brief GATT notification parameters
 *
 *  See also @ref bt_gatt_notify_cb and @ref bt_gatt_notify_multiple, using this parameter.
 */
struct bt_gatt_notify_params {
    /** @brief Notification Attribute UUID type
     *
     *  Optional, use to search for an attribute with matching UUID when
     *  the attribute object pointer is not known.
     */
    const struct bt_uuid *uuid;
    /** @brief Notification Attribute object
     *
     *  Optional if uuid is provided, in this case it will be used as start
     *  range to search for the attribute with the given UUID.
     */
    const struct bt_gatt_attr *attr;
    /** Notification Value data */
    const void *data;
    /** Notification Value length */
    uint16_t len;
    /** Notification Value callback */
    bt_gatt_complete_func_t func;
    /** Att channel options. */
    enum bt_att_chan_opt chan_opt;
};

/** @brief Notify attribute value change.
 *
 *  This function works in the same way as @ref bt_gatt_notify.
 *  With the addition that after sending the notification the
 *  callback function will be called.
 *
 *  The callback is run from System Workqueue context.
 *  When called from the System Workqueue context this API will not wait for
 *  resources for the callback but instead return an error.
 *
 *  Alternatively it is possible to notify by UUID by setting it on the
 *  parameters, when using this method the attribute if provided is used as the
 *  start range when looking up for possible matches.
 *
 *  @param conn Connection object.
 *  @param params Notification parameters.
 *
 *  @return 0 in case of success or negative value in case of error.
 */
int bt_gatt_notify_cb(struct bt_conn *conn,
                      struct bt_gatt_notify_params *params);

/** @brief Notify attribute value change.
 *
 *  Send notification of attribute value change, if connection is NULL notify
 *  all peer that have notification enabled via CCC otherwise do a direct
 *  notification only the given connection.
 *
 *  The attribute object on the parameters can be the so called Characteristic
 *  Declaration, which is usually declared with BT_GATT_CHARACTERISTIC followed
 *  by BT_GATT_CCC, or the Characteristic Value Declaration which is
 *  automatically created after the Characteristic Declaration when using
 *  BT_GATT_CHARACTERISTIC.
 *
 *  @param conn Connection object.
 *  @param attr Characteristic or Characteristic Value attribute.
 *  @param data Pointer to Attribute data.
 *  @param len Attribute value length.
 *
 *  @return 0 in case of success or negative value in case of error.
 */
static inline int bt_gatt_notify(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr,
                                 const void *data, uint16_t len)
{
    struct bt_gatt_notify_params params;

    memset(&params, 0, sizeof(params));

    params.attr = attr;
    params.data = data;
    params.len = len;
    params.chan_opt = BT_ATT_CHAN_OPT_NONE;

    return bt_gatt_notify_cb(conn, &params);
}

/** @brief Notify attribute value change by UUID.
 *
 *  Send notification of attribute value change, if connection is NULL notify
 *  all peer that have notification enabled via CCC otherwise do a direct
 *  notification only on the given connection.
 *
 *  The attribute object is the starting point for the search of the UUID.
 *
 *  @param conn Connection object.
 *  @param uuid The UUID. If the server contains multiple services with the same
 *              UUID, then the first occurrence, starting from the attr given,
 *              is used.
 *  @param attr Pointer to an attribute that serves as the starting point for
 *              the search of a match for the UUID.
 *  @param data Pointer to Attribute data.
 *  @param len  Attribute value length.
 *
 *  @return 0 in case of success or negative value in case of error.
 */
static inline int bt_gatt_notify_uuid(struct bt_conn *conn,
                                      const struct bt_uuid *uuid,
                                      const struct bt_gatt_attr *attr,
                                      const void *data, uint16_t len)
{
    struct bt_gatt_notify_params params;

    memset(&params, 0, sizeof(params));

    params.uuid = uuid;
    params.attr = attr;
    params.data = data;
    params.len = len;
    params.chan_opt = BT_ATT_CHAN_OPT_NONE;

    return bt_gatt_notify_cb(conn, &params);
}

/* Forward declaration of the bt_gatt_indicate_params structure */
struct bt_gatt_indicate_params;

/** @typedef bt_gatt_indicate_func_t
 *  @brief Indication complete result callback.
 *
 *  @param conn Connection object.
 *  @param params Indication params object.
 *  @param err ATT error code
 */
typedef void (*bt_gatt_indicate_func_t)(struct bt_conn *conn,
                                        struct bt_gatt_indicate_params *params,
                                        uint8_t err);

/** @brief GATT Indicate Value parameters
 *
 *  See also @ref bt_gatt_indicate, using this parameter.
 *
 */
struct bt_gatt_indicate_params {
    /** @brief Indicate Attribute UUID type
     *
     *  Optional, use to search for an attribute with matching UUID when
     *  the attribute object pointer is not known.
     */
    const struct bt_uuid *uuid;
    /** @brief Indicate Attribute object
     *
     *  Optional if uuid is provided, in this case it will be used as start
     *  range to search for the attribute with the given UUID.
     */
    const struct bt_gatt_attr *attr;
    /** Indicate Value callback */
    bt_gatt_indicate_func_t func;
    /** Indicate Value data*/
    const void *data;
    /** Indicate Value length*/
    uint16_t len;
    /** Att channel options. */
    enum bt_att_chan_opt chan_opt;
};

/** @brief Indicate attribute value change.
 *
 *  Send an indication of attribute value change. if connection is NULL
 *  indicate all peer that have notification enabled via CCC otherwise do a
 *  direct indication only the given connection.
 *
 *  The attribute object on the parameters can be the so called Characteristic
 *  Declaration, which is usually declared with BT_GATT_CHARACTERISTIC followed
 *  by BT_GATT_CCC, or the Characteristic Value Declaration which is
 *  automatically created after the Characteristic Declaration when using
 *  BT_GATT_CHARACTERISTIC.
 *
 *  Alternatively it is possible to indicate by UUID by setting it on the
 *  parameters, when using this method the attribute if provided is used as the
 *  start range when looking up for possible matches.
 *
 *  @note This procedure is asynchronous therefore the parameters need to
 *        remains valid while it is active. The procedure is active until
 *        the destroy callback is run.
 *
 *  @param conn Connection object.
 *  @param params Indicate parameters.
 *
 *  @return 0 in case of success or negative value in case of error.
 */
int bt_gatt_indicate(struct bt_conn *conn,
                     struct bt_gatt_indicate_params *params);

/** @brief Check if connection have subscribed to attribute
 *
 *  Check if the connection has subscribed to an attribute value change.
 *
 *  The attribute object can be the so called Characteristic Declaration,
 *  which is usually declared with BT_GATT_CHARACTERISTIC followed
 *  by BT_GATT_CCC, or the Characteristic Value Declaration which is
 *  automatically created after the Characteristic Declaration when using
 *  BT_GATT_CHARACTERISTIC, or the Client Characteristic Configuration
 *  Descriptor (CCCD) which is created by BT_GATT_CCC.
 *
 *  @param conn Connection object.
 *  @param attr Attribute object.
 *  @param ccc_type The subscription type, @ref BT_GATT_CCC_NOTIFY and/or
 *                  @ref BT_GATT_CCC_INDICATE.
 *
 *  @return true if the attribute object has been subscribed.
 */
bool bt_gatt_is_subscribed(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr, uint16_t ccc_type);

/** @brief Get ATT MTU for a connection
 *
 *  Get negotiated ATT connection MTU, note that this does not equal the largest
 *  amount of attribute data that can be transferred within a single packet.
 *
 *  @param conn Connection object.
 *
 *  @return MTU in bytes
 */
uint16_t bt_gatt_get_mtu(struct bt_conn *conn);

/** @} */

/**
 * @defgroup bt_gatt_client GATT Client APIs
 * @ingroup bt_gatt
 * @{
 */

/** @brief GATT Exchange MTU parameters
 *
 *  Used with @ref bt_gatt_exchange_mtu function to initiate an MTU exchange. The
 *  response is handled in the callback @p func, which is called upon
 *  completion from the 'config BT_RECV_CONTEXT' context.
 *
 *  @p params must remain valid until the callback executes.
 */
struct bt_gatt_exchange_params {
    /** Callback for MTU exchange response */
    void (*func)(struct bt_conn *conn, uint8_t err,
                 struct bt_gatt_exchange_params *params);
};

/** @brief Exchange MTU
 *
 *  Once per connection, this client procedure can be used to set the MTU to
 *  the maximum possible size the buffers can hold.
 *
 *  As the response comes in callback @p params->func, for example
 *  @ref bt_gatt_get_mtu can be invoked in the mtu_exchange-callback to read
 *  out the new negotiated ATT connection MTU. The callback is run from the
 *  context specified by 'config BT_RECV_CONTEXT' and @p params must remain
 *  valid until start of callback.
 *
 *  @param conn Connection object.
 *  @param params Exchange MTU parameters.
 *
 *  @retval 0 Successfully queued request. Will call @p params->func on
 *  resolution.
 *
 *  @retval -ENOMEM ATT request queue is full and blocking would cause deadlock.
 *  Allow a pending request to resolve before retrying, or call this function
 *  from a separate thread to get blocking behavior. Queue size is controlled
 *  by @kconfig{CONFIG_BT_ATT_TX_COUNT}.
 *
 *  @retval -EALREADY The MTU exchange procedure has been already performed.
 */
int bt_gatt_exchange_mtu(struct bt_conn *conn,
                         struct bt_gatt_exchange_params *params);

struct bt_gatt_discover_params;

/** @typedef bt_gatt_discover_func_t
 *  @brief Discover attribute callback function.
 *
 *  @param conn Connection object.
 *  @param attr Attribute found, or NULL if not found.
 *  @param params Discovery parameters given.
 *
 *  If discovery procedure has completed this callback will be called with
 *  attr set to NULL. This will not happen if procedure was stopped by returning
 *  BT_GATT_ITER_STOP.
 *
 *  The attribute object as well as its UUID and value objects are temporary and
 *  must be copied to in order to cache its information.
 *
 *  @note @ref bt_gatt_attr is given as an argument to @ref bt_gatt_discover function, but
 *  it's not a proper object of this type. @ref bt_gatt_attr.perm, and methods
 *  bt_gatt_attr.read() and bt_gatt_attr.write() are not available, and it's
 *  unsound to pass the pointer to GATT server APIs.
 *  Only the following fields of the attribute contains valid information:
 *   - uuid      UUID representing the type of attribute.
 *   - handle    Handle in the remote database.
 *   - user_data The value of the attribute, if the discovery type maps to an
 *               ATT operation that provides this information. NULL otherwise.
 *               See below.
 *
 *  The effective type of @c attr->user_data is determined by @c params. Note
 *  that the fields @c params->type and @c params->uuid are left unchanged by
 *  the discovery procedure.
 *
 *  @c params->type                      | @c params->uuid         | Type of @c attr->user_data
 *  -------------------------------------|-------------------------|---------------------------
 *  @ref BT_GATT_DISCOVER_PRIMARY        | any                     | @ref bt_gatt_service_val
 *  @ref BT_GATT_DISCOVER_SECONDARY      | any                     | @ref bt_gatt_service_val
 *  @ref BT_GATT_DISCOVER_INCLUDE        | any                     | @ref bt_gatt_include
 *  @ref BT_GATT_DISCOVER_CHARACTERISTIC | any                     | @ref bt_gatt_chrc
 *  @ref BT_GATT_DISCOVER_STD_CHAR_DESC  | @ref BT_UUID_GATT_CEP   | @ref bt_gatt_cep
 *  @ref BT_GATT_DISCOVER_STD_CHAR_DESC  | @ref BT_UUID_GATT_CCC   | @ref bt_gatt_ccc
 *  @ref BT_GATT_DISCOVER_STD_CHAR_DESC  | @ref BT_UUID_GATT_SCC   | @ref bt_gatt_scc
 *  @ref BT_GATT_DISCOVER_STD_CHAR_DESC  | @ref BT_UUID_GATT_CPF   | @ref bt_gatt_cpf
 *  @ref BT_GATT_DISCOVER_DESCRIPTOR     | any                     | NULL
 *  @ref BT_GATT_DISCOVER_ATTRIBUTE      | any                     | NULL
 *
 *  Also consider if using read-by-type instead of discovery is more convenient.
 *  See @ref bt_gatt_read with @ref bt_gatt_read_params.handle_count set to
 *  @c 0.
 *
 *  @return BT_GATT_ITER_CONTINUE to continue discovery procedure.
 *  @return BT_GATT_ITER_STOP to stop discovery procedure.
 */
typedef uint8_t (*bt_gatt_discover_func_t)(struct bt_conn *conn,
                                           const struct bt_gatt_attr *attr,
                                           struct bt_gatt_discover_params *params);

/** GATT Discover types */
enum bt_gatt_discover_type {
    /** Discover Primary Services. */
    BT_GATT_DISCOVER_PRIMARY,
    /** Discover Secondary Services. */
    BT_GATT_DISCOVER_SECONDARY,
    /** Discover Included Services. */
    BT_GATT_DISCOVER_INCLUDE,
    /** @brief Discover Characteristic Values.
     *
     *  Discover Characteristic Value and its properties.
     */
    BT_GATT_DISCOVER_CHARACTERISTIC,
    /** @brief Discover Descriptors.
     *
     *  Discover Attributes which are not services or characteristics.
     *
     *  @note The use of this type of discover is not recommended for
     *        discovering in ranges across multiple services/characteristics
     *        as it may incur in extra round trips.
     */
    BT_GATT_DISCOVER_DESCRIPTOR,
    /** @brief Discover Attributes.
     *
     *  Discover Attributes of any type.
     *
     *  @note The use of this type of discover is not recommended for
     *        discovering in ranges across multiple services/characteristics
     *        as it may incur in more round trips.
     */
    BT_GATT_DISCOVER_ATTRIBUTE,
    /** @brief Discover standard characteristic descriptor values.
     *
     *  Discover standard characteristic descriptor values and their
     *  properties.
     *  Supported descriptors:
     *   - Characteristic Extended Properties
     *   - Client Characteristic Configuration
     *   - Server Characteristic Configuration
     *   - Characteristic Presentation Format
     */
    BT_GATT_DISCOVER_STD_CHAR_DESC,
};

/** Handle value to denote that the CCC will be automatically discovered */
#define BT_GATT_AUTO_DISCOVER_CCC_HANDLE 0x0000U

/** @brief GATT Discover Attributes parameters */
struct bt_gatt_discover_params {
    /** Discover UUID type */
    const struct bt_uuid *uuid;
    /** Discover attribute callback */
    bt_gatt_discover_func_t func;
    union {
        /** See @ref bt_gatt_include for more on included services. */
        struct {
            /** Include service attribute declaration handle */
            uint16_t attr_handle;
            /** Starting attribute handle for included service */
            uint16_t start_handle;
            /** Ending attribute handle for included service */
            uint16_t end_handle;
        } _included;
        /** Starting attribute handle to begin discovery */
        uint16_t start_handle;
    };
    /** @brief Ending attribute handle to stop discovery at
     *
     * @note When discovery begins this can be set to
     * @ref BT_ATT_LAST_ATTRIBUTE_HANDLE to discover all attributes
     * in the service.
     */
    uint16_t end_handle;
    /** Discover type */
    uint8_t type;
    /** Only for stack-internal use, used for automatic discovery. */
    struct bt_gatt_subscribe_params *sub_params;
    /** Att channel options. */
    enum bt_att_chan_opt chan_opt;
};

/** @brief GATT Discover function
 *
 *  This procedure is used by a client to discover attributes on a server.
 *
 *  Primary Service Discovery: Procedure allows to discover primary services
 *                             either by Discover All Primary Services or
 *                             Discover Primary Services by Service UUID.
 *  Include Service Discovery: Procedure allows to discover all Include Services
 *                             within specified range.
 *  Characteristic Discovery:  Procedure allows to discover all characteristics
 *                             within specified handle range as well as
 *                             discover characteristics with specified UUID.
 *  Descriptors Discovery:     Procedure allows to discover all characteristic
 *                             descriptors within specified range.
 *
 *  For each attribute found the callback is called which can then decide
 *  whether to continue discovering or stop.
 *
 *  The Response comes in callback @p params->func. The callback is run from
 *  the BT RX thread. @p params must remain valid until start of callback where
 *  iter `attr` is `NULL` or callback will return `BT_GATT_ITER_STOP`.
 *
 *  @param conn Connection object.
 *  @param params Discover parameters.
 *
 *  @retval 0 Successfully queued request. Will call @p params->func on
 *  resolution.
 *
 *  @retval -ENOMEM ATT request queue is full and blocking would cause deadlock.
 *  Allow a pending request to resolve before retrying, or call this function
 *  from a separate thread to get blocking behavior. Queue size is controlled
 *  by @kconfig{CONFIG_BT_ATT_TX_COUNT}.
 */
int bt_gatt_discover(struct bt_conn *conn,
                     struct bt_gatt_discover_params *params);

struct bt_gatt_read_params;

/** @typedef bt_gatt_read_func_t
 *  @brief Read callback function
 *
 *  When reading using by_uuid, `params->start_handle` is the attribute handle
 *  for this `data` item.
 *
 *  If the received data length is invalid, the callback will called with the
 *  error @ref BT_ATT_ERR_INVALID_PDU.
 *
 *  @param conn Connection object.
 *  @param err ATT error code.
 *  @param params Read parameters used.
 *  @param data Attribute value data. NULL means read has completed.
 *  @param length Attribute value length.
 *
 *  @return BT_GATT_ITER_CONTINUE if should continue to the next attribute.
 *  @return BT_GATT_ITER_STOP to stop.
 */
typedef uint8_t (*bt_gatt_read_func_t)(struct bt_conn *conn, uint8_t err,
                                       struct bt_gatt_read_params *params,
                                       const void *data, uint16_t length);

/** @brief GATT Read parameters */
struct bt_gatt_read_params {
    /** Read attribute callback. */
    bt_gatt_read_func_t func;
    /** If equals to 1 single.handle and single.offset are used.
     *  If greater than 1 multiple.handles are used.
     *  If equals to 0 by_uuid is used for Read Using Characteristic UUID.
     */
    size_t handle_count;
    union {
        struct {
            /** Attribute handle. */
            uint16_t handle;
            /** Attribute data offset. */
            uint16_t offset;
        } single;
        struct {
            /** Attribute handles to read with Read Multiple
             *  Characteristic Values.
             */
            uint16_t *handles;
            /** If true use Read Multiple Variable Length
             *  Characteristic Values procedure.
             *  The values of the set of attributes may be of
             *  variable or unknown length.
             *  If false use Read Multiple Characteristic Values
             *  procedure.
             *  The values of the set of attributes must be of a
             *  known fixed length, with the exception of the last
             *  value that can have a variable length.
             */
            bool variable;
        } multiple;
        struct {
            /** @brief Requested start attribute handle number.
             *
             * @details The starting handle is set to the starting point of the range
             * over which this read should be performed. For example, this could be
             * set to @ref BT_ATT_FIRST_ATTRIBUTE_HANDLE to set the starting point of
             * the range at the beginning of the GATT database, or to the starting
             * handle of a service after discovery.
             *
             * This value is automatically incremented by the stack after
             * processing each matching handle-value pair returned by the server.
             */
            uint16_t start_handle;
            /** @brief Requested end attribute handle number.
             *
             * @details The end handle is set to the ending point of the range over
             * which this read should be performed. For example, this could be set to
             * @ref BT_ATT_LAST_ATTRIBUTE_HANDLE to set the ending point of the range
             * at the end of the GATT database, or to the end handle for service after
             * discovery, where the end_handle is available in the
             * @ref bt_gatt_service_val.
             */
            uint16_t end_handle;
            /** 2 or 16 octet UUID. */
            const struct bt_uuid *uuid;
        } by_uuid;
    };
    /** Att channel options. */
    enum bt_att_chan_opt chan_opt;
};

/** @brief Read Attribute Value by handle
 *
 *  This procedure reads the attribute value and returns it to the callback.
 *
 *  When reading attributes by UUID the callback can be called multiple times
 *  depending on how many instances of a given UUID exists with the start_handle
 *  being updated for each instance.
 *
 *  To perform a GATT Long Read procedure, start with a Characteristic Value
 *  Read (by setting @c offset @c 0 and @c handle_count @c 1) and then return
 *  @ref BT_GATT_ITER_CONTINUE from the callback. This is equivalent to calling
 *  @ref bt_gatt_read again, but with the correct offset to continue the read.
 *  This may be repeated until the procedure is complete, which is signaled by
 *  the callback being called with @p data set to @c NULL.
 *
 *  Note that returning @ref BT_GATT_ITER_CONTINUE is really starting a new ATT
 *  operation, so this can fail to allocate resources. However, all API errors
 *  are reported as if the server returned @ref BT_ATT_ERR_UNLIKELY. There is no
 *  way to distinguish between this condition and a @ref BT_ATT_ERR_UNLIKELY
 *  response from the server itself.
 *
 *  Note that the effect of returning @ref BT_GATT_ITER_CONTINUE from the
 *  callback varies depending on the type of read operation.
 *
 *  The Response comes in callback @p params->func. The callback is run from
 *  the context specified by 'config BT_RECV_CONTEXT'.
 *  @p params must remain valid until start of callback.
 *  If the received data length is invalid, the callback @p params->func will
 *  called with the error @ref BT_ATT_ERR_INVALID_PDU.
 *
 *  @param conn Connection object.
 *  @param params Read parameters.
 *
 *  @retval 0 Successfully queued request. Will call @p params->func on
 *  resolution.
 *
 *  @retval -ENOMEM ATT request queue is full and blocking would cause deadlock.
 *  Allow a pending request to resolve before retrying, or call this function
 *  from a separate thread to get blocking behavior. Queue size is controlled
 *  by @kconfig{CONFIG_BT_ATT_TX_COUNT}.
 */
int bt_gatt_read(struct bt_conn *conn, struct bt_gatt_read_params *params);

struct bt_gatt_write_params;

/** @typedef bt_gatt_write_func_t
 *  @brief Write callback function
 *
 *  @param conn Connection object.
 *  @param err ATT error code.
 *  @param params Write parameters used.
 */
typedef void (*bt_gatt_write_func_t)(struct bt_conn *conn, uint8_t err,
                                     struct bt_gatt_write_params *params);

/** @brief GATT Write parameters */
struct bt_gatt_write_params {
    /** Response callback */
    bt_gatt_write_func_t func;
    /** Attribute handle */
    uint16_t handle;
    /** Attribute data offset */
    uint16_t offset;
    /** Data to be written */
    const void *data;
    /** Length of the data */
    uint16_t length;
    /** Att channel options. */
    enum bt_att_chan_opt chan_opt;
};

/** @brief Write Attribute Value by handle
 *
 *  The Response comes in callback @p params->func. The callback is run from
 *  the context specified by 'config BT_RECV_CONTEXT'.
 *  @p params must remain valid until start of callback.
 *
 *  @param conn Connection object.
 *  @param params Write parameters.
 *
 *  @retval 0 Successfully queued request. Will call @p params->func on
 *  resolution.
 *
 *  @retval -ENOMEM ATT request queue is full and blocking would cause deadlock.
 *  Allow a pending request to resolve before retrying, or call this function
 *  from a separate thread to get blocking behavior. Queue size is controlled
 *  by @kconfig{CONFIG_BT_ATT_TX_COUNT}.
 */
int bt_gatt_write(struct bt_conn *conn, struct bt_gatt_write_params *params);

/** @brief Write Attribute Value by handle without response with callback.
 *
 *  This function works in the same way as @ref bt_gatt_write_without_response.
 *  With the addition that after sending the write the callback function will be
 *  called.
 *
 *  The callback is run from System Workqueue context.
 *  When called from the System Workqueue context this API will not wait for
 *  resources for the callback but instead return an error.
 *
 *  @param conn Connection object.
 *  @param handle Attribute handle.
 *  @param data Data to be written.
 *  @param length Data length.
 *  @param sign Whether to sign data.
 *  @param func Transmission complete callback.
 *  @param user_data User data to be passed back to callback.
 *
 *  @retval 0 Successfully queued request.
 *
 *  @retval -ENOMEM ATT request queue is full and blocking would cause deadlock.
 *  Allow a pending request to resolve before retrying, or call this function
 *  from a separate thread to get blocking behavior. Queue size is controlled
 *  by @kconfig{CONFIG_BT_ATT_TX_COUNT}.
 */
int bt_gatt_write_without_response_cb(struct bt_conn *conn, uint16_t handle,
                                      const void *data, uint16_t length,
                                      bool sign, bt_gatt_complete_func_t func,
                                      void *user_data);

/** @brief Write Attribute Value by handle without response
 *
 *  This procedure write the attribute value without requiring an
 *  acknowledgment that the write was successfully performed
 *
 *  @param conn Connection object.
 *  @param handle Attribute handle.
 *  @param data Data to be written.
 *  @param length Data length.
 *  @param sign Whether to sign data.
 *
 *  @retval 0 Successfully queued request.
 *
 *  @retval -ENOMEM ATT request queue is full and blocking would cause deadlock.
 *  Allow a pending request to resolve before retrying, or call this function
 *  from a separate thread to get blocking behavior. Queue size is controlled
 *  by @kconfig{CONFIG_BT_ATT_TX_COUNT}.
 */
static inline int bt_gatt_write_without_response(struct bt_conn *conn,
                                                 uint16_t handle, const void *data,
                                                 uint16_t length, bool sign)
{
    return bt_gatt_write_without_response_cb(conn, handle, data, length,
                                             sign, NULL, NULL);
}

struct bt_gatt_subscribe_params;

/** @typedef bt_gatt_notify_func_t
 *  @brief Notification callback function
 *
 *  In the case of an empty notification, the @p data pointer will be non-NULL
 *  while the @p length will be 0, which is due to the special case where
 *  a @p data NULL pointer means unsubscribed.
 *
 *  @param conn Connection object. May be NULL, indicating that the peer is
 *              being unpaired
 *  @param params Subscription parameters.
 *  @param data Attribute value data. If NULL then subscription was removed.
 *  @param length Attribute value length.
 *
 *  @return BT_GATT_ITER_CONTINUE to continue receiving value notifications.
 *          BT_GATT_ITER_STOP to unsubscribe from value notifications.
 */
typedef uint8_t (*bt_gatt_notify_func_t)(struct bt_conn *conn,
                                         struct bt_gatt_subscribe_params *params,
                                         const void *data, uint16_t length);

/** @typedef bt_gatt_subscribe_func_t
 *  @brief Subscription callback function
 *
 *  @param conn Connection object.
 *  @param err ATT error code.
 *  @param params Subscription parameters used.
 */
typedef void (*bt_gatt_subscribe_func_t)(struct bt_conn *conn, uint8_t err,
                                         struct bt_gatt_subscribe_params *params);

/** Subscription flags */
enum bt_gatt_sub_flag {
    /** @brief Persistence flag
     *
     *  If set, indicates that the subscription is not saved
     *  on the GATT server side. Therefore, upon disconnection,
     *  the subscription will be automatically removed
     *  from the client's subscriptions list and
     *  when the client reconnects, it will have to
     *  issue a new subscription.
     */
    BT_GATT_SUBSCRIBE_FLAG_VOLATILE,

    /** @brief No resubscribe flag
     *
     *  By default when BT_GATT_SUBSCRIBE_FLAG_VOLATILE is unset, the
     *  subscription will be automatically renewed when the client
     *  reconnects, as a workaround for GATT servers that do not persist
     *  subscriptions.
     *
     *  This flag will disable the automatic resubscription. It is useful
     *  if the application layer knows that the GATT server remembers
     *  subscriptions from previous connections and wants to avoid renewing
     *  the subscriptions.
     */
    BT_GATT_SUBSCRIBE_FLAG_NO_RESUB,

    /** @brief Write pending flag
     *
     *  If set, indicates write operation is pending waiting remote end to
     *  respond.
     *
     *  @note Internal use only.
     */
    BT_GATT_SUBSCRIBE_FLAG_WRITE_PENDING,

    /** @brief Sent flag
     *
     *  If set, indicates that a subscription request (CCC write) has
     *  already been sent in the active connection.
     *
     *  Used to avoid sending subscription requests multiple times when the
     *  @kconfig{CONFIG_BT_GATT_AUTO_RESUBSCRIBE} quirk is enabled.
     *
     *  @note Internal use only.
     */
    BT_GATT_SUBSCRIBE_FLAG_SENT,

    BT_GATT_SUBSCRIBE_NUM_FLAGS
};

/** @brief GATT Subscribe parameters */
struct bt_gatt_subscribe_params {
    /** Notification value callback */
    bt_gatt_notify_func_t notify;
    /** Subscribe CCC write request response callback
     *  If given, called with the subscription parameters given when subscribing
     */
    bt_gatt_subscribe_func_t subscribe;

    /** Subscribe value handle */
    uint16_t value_handle;
    /** Subscribe CCC handle */
    uint16_t ccc_handle;
    /** Subscribe End handle (for automatic discovery) */
    uint16_t end_handle;
    /** Discover parameters used when ccc_handle = @ref BT_GATT_AUTO_DISCOVER_CCC_HANDLE */
    struct bt_gatt_discover_params *disc_params;
    /** Subscribe value */
    uint16_t value;
    /** Subscription flags, see @ref bt_gatt_sub_flag */
    ATOMIC_DEFINE(flags, BT_GATT_SUBSCRIBE_NUM_FLAGS);

    sys_snode_t node;
    /** Att channel options. */
    enum bt_att_chan_opt chan_opt;
};

/** @brief Subscribe Attribute Value Notification
 *
 *  This procedure subscribe to value notification using the Client
 *  Characteristic Configuration handle.
 *  If notification received subscribe value callback is called to return
 *  notified value. One may then decide whether to unsubscribe directly from
 *  this callback. Notification callback with NULL data will not be called if
 *  subscription was removed by this method.
 *
 *  The Response comes in callback @p params->subscribe. The callback is run from
 *  the context specified by 'config BT_RECV_CONTEXT'.
 *  The Notification callback @p params->notify is also called from the BT RX
 *  thread.
 *
 *  @note Notifications are asynchronous therefore the @p params must remain
 *        valid while subscribed and cannot be reused for additional subscriptions
 *        whilst active.
 *
 *  @param conn Connection object.
 *  @param params Subscribe parameters.
 *
 *  @retval 0 Successfully queued request. Will call @p params->write on
 *  resolution.
 *
 *  @retval -ENOMEM ATT request queue is full and blocking would cause deadlock.
 *  Allow a pending request to resolve before retrying, or call this function
 *  from a separate thread to get blocking behavior. Queue size is controlled
 *  by @kconfig{CONFIG_BT_ATT_TX_COUNT}.
 *
 *  @retval -EALREADY if there already exist a subscription using the @p params.
 *
 *  @retval -EBUSY if @p params.ccc_handle is 0 and @kconfig{CONFIG_BT_GATT_AUTO_DISCOVER_CCC} is
 *  enabled and discovery for the @p params is already in progress.
 */
int bt_gatt_subscribe(struct bt_conn *conn,
                      struct bt_gatt_subscribe_params *params);

/** @brief Unsubscribe Attribute Value Notification
 *
 *  This procedure unsubscribe to value notification using the Client
 *  Characteristic Configuration handle. Notification callback with NULL data
 *  will be called if subscription was removed by this call, until then the
 *  parameters cannot be reused.
 *
 *  The Response comes in callback @p params->func. The callback is run from
 *  the BT RX thread.
 *
 *  @param conn Connection object.
 *  @param params Subscribe parameters. The parameters shall be a @ref bt_gatt_subscribe_params from
 *                a previous call to bt_gatt_subscribe().
 *
 *  @retval 0 Successfully queued request. Will call @p params->write on
 *  resolution.
 *
 *  @retval -ENOMEM ATT request queue is full and blocking would cause deadlock.
 *  Allow a pending request to resolve before retrying, or call this function
 *  from a separate thread to get blocking behavior. Queue size is controlled
 *  by @kconfig{CONFIG_BT_ATT_TX_COUNT}.
 */
int bt_gatt_unsubscribe(struct bt_conn *conn,
                        struct bt_gatt_subscribe_params *params);

/** @} */

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_GATT_H_ */
