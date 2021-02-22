// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright © 2019 Oracle and/or its affiliates. All rights reserved.
 * Copyright © 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * KVM Xen emulation
 */

#ifndef __ARCH_X86_KVM_XEN_H__
#define __ARCH_X86_KVM_XEN_H__

#include <linux/jump_label_ratelimit.h>

extern struct static_key_false_deferred kvm_xen_enabled;

int __kvm_xen_has_interrupt(struct kvm_vcpu *vcpu);
int kvm_xen_vcpu_set_attr(struct kvm_vcpu *vcpu, struct kvm_xen_vcpu_attr *data);
int kvm_xen_vcpu_get_attr(struct kvm_vcpu *vcpu, struct kvm_xen_vcpu_attr *data);
int kvm_xen_hvm_set_attr(struct kvm *kvm, struct kvm_xen_hvm_attr *data);
int kvm_xen_hvm_get_attr(struct kvm *kvm, struct kvm_xen_hvm_attr *data);
int kvm_xen_hypercall(struct kvm_vcpu *vcpu);
int kvm_xen_write_hypercall_page(struct kvm_vcpu *vcpu, u64 data);
int kvm_xen_hvm_config(struct kvm *kvm, struct kvm_xen_hvm_config *xhc);
void kvm_xen_destroy_vm(struct kvm *kvm);

static inline bool kvm_xen_hypercall_enabled(struct kvm *kvm)
{
	return static_branch_unlikely(&kvm_xen_enabled.key) &&
		(kvm->arch.xen_hvm_config.flags &
		 KVM_XEN_HVM_CONFIG_INTERCEPT_HCALL);
}

static inline int kvm_xen_has_interrupt(struct kvm_vcpu *vcpu)
{
	if (static_branch_unlikely(&kvm_xen_enabled.key) &&
	    vcpu->arch.xen.vcpu_info_set && vcpu->kvm->arch.xen.upcall_vector)
		return __kvm_xen_has_interrupt(vcpu);

	return 0;
}

/* 32-bit compatibility definitions, also used natively in 32-bit build */
#include <asm/pvclock-abi.h>
#include <asm/xen/interface.h>

struct compat_arch_vcpu_info {
	unsigned int cr2;
	unsigned int pad[5];
};

struct compat_vcpu_info {
	uint8_t evtchn_upcall_pending;
	uint8_t evtchn_upcall_mask;
	uint16_t pad;
	uint32_t evtchn_pending_sel;
	struct compat_arch_vcpu_info arch;
	struct pvclock_vcpu_time_info time;
}; /* 64 bytes (x86) */

struct compat_arch_shared_info {
	unsigned int max_pfn;
	unsigned int pfn_to_mfn_frame_list_list;
	unsigned int nmi_reason;
	unsigned int p2m_cr3;
	unsigned int p2m_vaddr;
	unsigned int p2m_generation;
	uint32_t wc_sec_hi;
};

struct compat_shared_info {
	struct compat_vcpu_info vcpu_info[MAX_VIRT_CPUS];
	uint32_t evtchn_pending[32];
	uint32_t evtchn_mask[32];
	struct pvclock_wall_clock wc;
	struct compat_arch_shared_info arch;
};

#endif /* __ARCH_X86_KVM_XEN_H__ */
