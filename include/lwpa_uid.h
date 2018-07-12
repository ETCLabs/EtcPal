/******************************************************************************
 * Copyright 2018 ETC Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************
 * This file is a part of lwpa. For more information, go to:
 * https://github.com/ETCLabs/lwpa
 ******************************************************************************/

/* lwpa_uid.h: A type and helper functions for the Unique ID (UID) used in the
 * RDM family of protocols. */
#ifndef _LWPA_UID_H_
#define _LWPA_UID_H_

#include "lwpa_int.h"

/*! \defgroup lwpa_uid lwpa_uid
 *  \ingroup lwpa
 *  \brief Type and helper functions for the Unique ID (UID) used in the RDM
 *         family of protocols.
 *
 *  RDM-based protocols use an identifier called a UID to identify an entity
 *  that generates or consumes RDM messages.
 *
 *  The top bit of the UID is reserved for indicating broadcast values, or
 *  indicating Dynamic UIDs in ANSI E1.33.
 *
 *  The next 15 bits is the ESTA Manufacturer ID; each manufacturer of RDM
 *  equipment has a unique Manufacturer ID registered with ESTA. The MSB of the
 *  UID is reserved for indicating broadcast values, or indicating Dynamic UIDs
 *  in ANSI E1.33.
 *
 *  The remaining 32 bits is the Device ID. The Device ID is unique within a
 *  given scope; in the case of a Static UID, it is unique across all products
 *  made by the manufacturer indicated by the ESTA Manufacturer ID. In the case
 *  of a Dynamic UID, it is unique within its current E1.33 Scope.
 *
 *  \#include "lwpa_uid.h"
 *
 *  @{
 */

/*! The UID type. Don't forget that the top bit of the manu field has special
 *  meaning. The helper macros are your friend! */
typedef struct LwpaUid
{
  uint16_t manu;
  uint32_t id;
} LwpaUid;

/*************************** UID Comparison Macros ***************************/

/*! \brief Compare two UIDs.
 *  \param uidptr1 Pointer to first LwpaUid to compare.
 *  \param uidptr2 Pointer to second LwpaUid to compare.
 *  \return < 0 (uidptr1 is less than uidptr2)\n
 *            0 (uidptr1 is equal to uidptr2)\n
 *          > 0 (uidptr1 is greater than uidptr2)
 */
#define uidcmp(uidptr1, uidptr2)                                                    \
  (((uidptr1)->manu == (uidptr2)->manu) ? ((int)(uidptr1)->id - (int)(uidptr2)->id) \
                                        : ((int)(uidptr1)->manu - (int)(uidptr2)->manu))

/*! \brief Determine if two UIDs are equal.
 *  \param uidptr1 Pointer to first struct lwpa_uid.
 *  \param uidptr2 Pointer to second struct lwpa_uid.
 *  \return true (UIDs are equal) or false (UIDs are not equal).
 */
#define uid_equal(uidptr1, uidptr2) ((uidptr1)->manu == (uidptr2)->manu && (uidptr1)->id == (uidptr2)->id)

/************************* UID Initialization Macros *************************/

/*! Initialize a Static UID with a Manufacturer ID and Device ID.
 *  \param uidptr Pointer to LwpaUid to initialize.
 *  \param manu_val ESTA Manufacturer ID.
 *  \param id_val Device ID.
 */
#define init_static_uid(uidptr, manu_val, id_val) \
  do                                              \
  {                                               \
    (uidptr)->manu = (manu_val);                  \
    (uidptr)->id = (id_val);                      \
  } while (0)

/*! Initialize a Dynamic UID with a Manufacturer ID and Device ID.
 *  \param uidptr Pointer to LwpaUid to initialize.
 *  \param manu_val ESTA Manufacturer ID.
 *  \param id_val Device ID.
 */
#define init_dynamic_uid(uidptr, manu_val, id_val) \
  do                                               \
  {                                                \
    (uidptr)->manu = (0x8000u | (manu_val));       \
    (uidptr)->id = (id_val);                       \
  } while (0)

/**************************** UID Broadcast Macros ***************************/

/*! \brief Determine whether a UID is the E1.20 value BROADCAST_ALL_DEVICES_ID.
 *  \param uidptr Pointer to LwpaUid to check.
 *  \return true (uidptr is equal to BROADCAST_ALL_DEVICES_ID) or false (uidptr
 *          is not equal to BROADCAST_ALL_DEVICES_ID).
 */
#define uid_is_broadcast(uidptr) ((uidptr)->manu == kBroadcastUid.manu && (uidptr)->id == kBroadcastUid.id)

/*! \brief Determine whether a UID is the E1.33 value RPT_ALL_CONTROLLERS.
 *  \param uidptr Pointer to LwpaUid to check.
 *  \return true (uidptr is equal to RPT_ALL_CONTROLLERS) or false (uidptr is
 *          not equal to RPT_ALL_CONTROLLERS).
 */
#define uid_is_rdmnet_controller_broadcast(uidptr) \
  ((uidptr)->manu == kRdmnetControllerBroadcastUid.manu && (uidptr)->id == kRdmnetControllerBroadcastUid.id)

/*! \brief Determine whether a UID is the E1.33 value RPT_ALL_DEVICES.
 *  \param uidptr Pointer to LwpaUid to check.
 *  \return true (uidptr is equal to RPT_ALL_DEVICES) or false (uidptr is not
 *          equal to RPT_ALL_DEVICES).
 */
#define uid_is_rdmnet_device_broadcast(uidptr) \
  ((uidptr)->manu == kRdmnetDeviceBroadcastUid.manu && (uidptr)->id == kRdmnetDeviceBroadcastUid.id)

/*! \brief Determine whether a UID is one of the E1.33 values defined by
 *         RPT_ALL_MID_DEVICES.
 *
 *  RDMnet Device Manufacturer Broadcasts are directed to all Devices with a
 *  specific ESTA Manufacturer ID. If this check returns true, use
 *  rdmnet_device_broadcast_manu_id() to determine which Manufacturer ID this
 *  broadcast is directed to, or rdmnet_device_broadcast_manu_matches() to
 *  determine whether it matches a specific Manufacturer ID.
 *
 *  \param uidptr Pointer to LwpaUid to check.
 *  \return true (uidptr is one of the values defined by RPT_ALL_MID_DEVICES)
 *          or false (uidptr is not one of the values defined by
 *          RPT_ALL_MID_DEVICES).
 */
#define uid_is_rdmnet_device_manu_broadcast(uidptr) \
  ((uidptr)->manu == kRdmnetControllerBroadcastUid.manu && (((uidptr)->id & 0xffffu) == 0xffffu))

/*! \brief Determine whether an RDMnet Device Manufacturer Broadcast UID
 *         matches a specific ESTA Manufacturer ID.
 *
 *  Use uid_is_rdmnet_device_manu_broadcast() first to determine whether this
 *  UID is an RDMnet Device Manufacturer Broadcast.
 *
 *  \param uidptr Pointer to LwpaUid to check.
 *  \param manu_val ESTA Manufacturer ID to check against the Broadcast UID.
 *  \return true (RDMnet Device Manufacturer Broadcast matches this
 *          Manufacturer ID) or false (RDMnet Device Manufacturer Broadcast
 *          does not match this Manufacturer ID).
 */
#define rdmnet_device_broadcast_manu_matches(uidptr, manu_val) (rdmnet_device_broadcast_manu_id(uidptr) == manu_val)

/*! \brief Get the ESTA Manufacturer ID from an RDMnet Device Manufacturer
 *         Broadcast UID.
 *
 *  Use uid_is_rdmnet_device_manu_broadcast() first to determine whether this
 *  UID is an RDMnet Device Manufacturer Broadcast.
 *
 *  \param uidptr Pointer to LwpaUid from which to get the Broadcast
 *                Manufacturer ID.
 *  \return ESTA Manufacturer ID.
 */
#define rdmnet_device_broadcast_manu_id(uidptr) ((uint16_t)((uidptr)->id >> 16))

/******************************* UID Inspection ******************************/

/*! \brief Determine whether a UID is a Dynamic UID as defined in ANSI E1.33.
 *
 *  \param uidptr Pointer to LwpaUid to check.
 *  \return true (UID is an E1.33 Dynamic UID) or false (UID is not an E1.33
 *          Dynamic UID).
 */
#define uid_is_dynamic(uidptr)                                                         \
  ((((uidptr)->manu & 0x8000u) != 0) && !uid_is_rdmnet_controller_broadcast(uidptr) && \
   !uid_is_rdmnet_device_manu_broadcast(uidptr) && !uid_is_broadcast(uidptr))

/*! \brief Get the ESTA Manufacturer ID from a UID.
 *
 *  \param uidptr Pointer to LwpaUid from which to get the ESTA
 *         Manufacturer ID.
 *  \return ESTA Manufacturer ID.
 */
#define get_manufacturer_id(uidptr) ((uidptr)->manu & 0x7fffu)

/*! \brief Get the Device ID from a UID.
 *
 *  \param uidptr Pointer to LwpaUid from which to get the Device ID.
 *  \return Device ID.
 */
#define get_device_id(uidptr) ((uidptr)->id)

#ifdef __cplusplus
extern "C" {
#endif

/*! A UID that is equal to BROADCAST_ALL_DEVICES_ID as defined in ANSI
 *  E1.20. */
extern const LwpaUid kBroadcastUid;

/*! A UID that is equal to RPT_ALL_CONTROLLERS as defined in ANSI E1.33. */
extern const LwpaUid kRdmnetControllerBroadcastUid;

/*! A UID that is equal to RPT_ALL_DEVICES as defined in ANSI E1.33. */
extern const LwpaUid kRdmnetDeviceBroadcastUid;

#ifdef __cplusplus
}
#endif

/*!@}*/

#endif /* _LWPA_UID_H_ */
