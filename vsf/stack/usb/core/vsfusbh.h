#ifndef __VSFUSBH_H_INCLUDED__
#define __VSFUSBH_H_INCLUDED__

#include "vsfusbh_cfg.h"
#include "stack/usb/common/usb_common.h"
#include "stack/usb/common/usb_ch11.h"
#include "hcd.h"

#include "tool/list/list.h"
#include "tool/buffer/buffer.h"
#include "tool/bittool/bittool.h"
#include "framework/vsfsm/vsfsm.h"
#include "framework/vsftimer/vsftimer.h"

#define VSFSM_EVT_URB_COMPLETE	(VSFSM_EVT_USER + 1)

#define DEFAULT_TIMEOUT			50	// 50ms

struct vsfusbh_device_t
{
	uint8_t devnum;
	uint8_t devnum_temp;
	uint16_t speed;		/* full/low/high */

	struct usb_device_descriptor_t descriptor;
	struct usb_config_t *config;
	struct usb_config_t *actconfig;

	uint32_t toggle[2];	// one bit per endpoint
	uint32_t halted[2];	// one bit per endpoint
	
	uint16_t epmaxpacketin[USB_MAXENDPOINTS];
	uint16_t epmaxpacketout[USB_MAXENDPOINTS];

	struct vsfusbh_device_t *parent;
	struct vsfusbh_device_t *children[USB_MAXCHILDREN];
	uint16_t maxchild;
	uint16_t portnr;

	uint8_t act_config;
	uint8_t act_interface;
	uint8_t alternate_interface;

	// TODO
	// *deiver[num]
	// driver_cnt

	// save hcd pointer
	void *priv;
};

struct iso_packet_descriptor_t
{
	uint32_t offset;				/*!< Start offset in transfer buffer	*/
	uint32_t length;				/*!< Length in transfer buffer			*/
	uint32_t actual_length;			/*!< Actual transfer length				*/
	int32_t status;					/*!< Transfer status					*/
};


/* The following flags are used internally by usbcore and HCDs */
#define URB_DIR_IN		0x0200	/* Transfer from device to host */
#define URB_DIR_OUT		0
#define URB_DIR_MASK		URB_DIR_IN

#define URB_DMA_MAP_SINGLE	0x00010000	/* Non-scatter-gather mapping */
#define URB_DMA_MAP_PAGE	0x00020000	/* HCD-unsupported S-G */
#define URB_DMA_MAP_SG		0x00040000	/* HCD-supported S-G */
#define URB_MAP_LOCAL		0x00080000	/* HCD-local-memory mapping */
#define URB_SETUP_MAP_SINGLE	0x00100000	/* Setup packet DMA mapped */
#define URB_SETUP_MAP_LOCAL	0x00200000	/* HCD-local setup packet */
#define URB_DMA_SG_COMBINED	0x00400000	/* S-G entries were combined */
#define URB_ALIGNED_TEMP_BUFFER	0x00800000	/* Temp buffer was alloc'd */

// struct vsfusbh_urb_t.transfer_flags
#define USB_DISABLE_SPD			0x0001
#define URB_SHORT_NOT_OK		USB_DISABLE_SPD
#define USB_ISO_ASAP			0x0002
#define USB_ASYNC_UNLINK		0x0008
#define USB_QUEUE_BULK			0x0010
#define USB_NO_FSBR				0x0020
#define USB_ZERO_PACKET			0x0040		// Finish bulk OUTs always with zero length packet
#define URB_NO_INTERRUPT		0x0080		/* HINT: no non-error interrupt needed */
/* ... less overhead for QUEUE_BULK */
#define USB_TIMEOUT_KILLED		0x1000		// only set by HCD!
#define URB_ZERO_PACKET			USB_ZERO_PACKET
#define URB_ISO_ASAP			USB_ISO_ASAP

struct vsfusbh_urb_t
{
	struct urb_priv_t *urb_priv;

	struct vsfusbh_device_t *vsfdev;
	uint32_t pipe;					/*!< pipe information						*/
	int32_t status;					/*!< returned status						*/

	uint32_t transfer_flags;		/*!< USB_DISABLE_SPD | USB_ISO_ASAP | etc.	*/
	void *transfer_buffer;
	uint32_t transfer_length;
	uint32_t actual_length;

	struct usb_ctrlrequest_t setup_packet;

	uint32_t start_frame;			/*!< start frame (iso/irq only)		*/
	uint32_t interval;				/*!< polling interval (irq only)	*/
#if VSFUSBH_CFG_ENABLE_ISO
	uint32_t number_of_packets;		/*!< number of packets (iso)		*/
	uint32_t error_count;			/*!< number of errors (iso only)	*/
	struct iso_packet_descriptor_t iso_frame_desc[8];
#endif // VSFUSBH_CFG_ENABLE_ISO

	uint32_t timeout;
	struct vsfsm_t *sm;

	struct sllist list;

	// unused
	//void *context;				/*!< USB Driver internal used		*/
};

struct vsfusbh_t;
struct vsfusbh_class_drv_t
{
	void * (*init)(struct vsfusbh_t *usbh, struct vsfusbh_device_t *dev);
	void(*free)(struct vsfusbh_device_t *dev);
	vsf_err_t(*match)(struct vsfusbh_device_t *dev);
};

struct vsfusbh_class_data_t
{
	const struct vsfusbh_class_drv_t *drv;

	struct vsfusbh_device_t *dev;
	void *param;

	struct sllist list;
};

struct vsfusbh_hcddrv_t
{
	vsf_err_t (*init_thread)(struct vsfsm_pt_t *pt, vsfsm_evt_t evt);
	vsf_err_t (*fini)(void* param);
	vsf_err_t (*suspend)(void* param);
	vsf_err_t (*resume)(void* param);
	vsf_err_t (*alloc_device)(void *param, struct vsfusbh_device_t *dev);
	vsf_err_t (*free_device)(void *param, struct vsfusbh_device_t *dev);
	vsf_err_t (*submit_urb)(void *param, struct vsfusbh_urb_t *vsfurb);
	vsf_err_t (*unlink_urb)(void *param, struct vsfusbh_urb_t *vsfurb,
			void *delay_free_buf);
	vsf_err_t (*relink_urb)(void *param, struct vsfusbh_urb_t *vsfurb);
	vsf_err_t (*roothub_control)(void *param, struct vsfusbh_urb_t *vsfurb);
};

struct vsfusbh_t
{
	const struct vsfusbh_hcddrv_t *hcd;

	// private
	void *hcd_data; // print to 'struct vsfohci_t *vsfohci'
	uint32_t device_bitmap[4];
	struct vsfusbh_device_t *rh_dev;
	struct vsfusbh_device_t *new_dev;
	struct sllist drv_list;
	struct sllist dev_list;

	struct vsfsm_t sm;
	struct vsfsm_pt_t dev_probe_pt;
	struct vsfsm_pt_t hcd_init_pt;

	struct vsfusbh_urb_t probe_urb;
};

vsf_err_t vsfusbh_submit_urb(struct vsfusbh_t *usbh, struct vsfusbh_urb_t *vsfurb);
vsf_err_t vsfusbh_unlink_urb(struct vsfusbh_t *usbh, struct vsfusbh_urb_t *vsfurb);
vsf_err_t vsfusbh_relink_urb(struct vsfusbh_t *usbh, struct vsfusbh_urb_t *vsfurb);

vsf_err_t vsfusbh_init(struct vsfusbh_t *usbh);
vsf_err_t vsfusbh_fini(struct vsfusbh_t *usbh);
vsf_err_t vsfusbh_register_driver(struct vsfusbh_t *usbh,
		const struct vsfusbh_class_drv_t *drv);
struct vsfusbh_device_t *vsfusbh_alloc_device(struct vsfusbh_t *usbh);
void vsfusbh_free_device(struct vsfusbh_t *usbh, struct vsfusbh_device_t *dev);
vsf_err_t vsfusbh_add_device(struct vsfusbh_t *usbh,
		struct vsfusbh_device_t *dev);
void vsfusbh_remove_device(struct vsfusbh_t *usbh,
		struct vsfusbh_device_t *dev);

vsf_err_t vsfusbh_control_msg(struct vsfusbh_t *usbh, struct vsfusbh_urb_t *vsfurb,
		uint8_t bRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex);

vsf_err_t vsfusbh_set_address(struct vsfusbh_t *usbh,
		struct vsfusbh_urb_t *vsfurb);
vsf_err_t vsfusbh_get_descriptor(struct vsfusbh_t *usbh,
		struct vsfusbh_urb_t *vsfurb, uint8_t type, uint8_t index);
vsf_err_t vsfusbh_set_configuration(struct vsfusbh_t *usbh,
		struct vsfusbh_urb_t *vsfurb, uint8_t configuration);
vsf_err_t vsfusbh_set_interface(struct vsfusbh_t *usbh,
		struct vsfusbh_urb_t *vsfurb, uint16_t interface, uint16_t alternate);

#endif	// __VSFUSBH_H_INCLUDED__
