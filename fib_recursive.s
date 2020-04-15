	.def	 _fib;
	.scl	2;
	.type	32;
	.endef
	.section	.rdata,"r"
	.align	8
LCPI0_0:
	.quad	4607182418800017408     # double 1.000000e+00
                                        #  (0x0)
LCPI0_1:
	.quad	-4616189618054758400    # double -1.000000e+00
                                        #  (0x0)
LCPI0_2:
	.quad	-4611686018427387904    # double -2.000000e+00
                                        #  (0x0)
	.text
	.globl	_fib
	.align	16, 0x90
_fib:                                   # @fib
# BB#0:                                 # %func
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-8, %esp
	subl	$40, %esp
	movsd	8(%ebp), %xmm0
	movsd	%xmm0, 24(%esp)
	ucomisd	LCPI0_0, %xmm0
	ja	LBB0_2
# BB#1:                                 # %then
	movl	$1072693248, 36(%esp)   # imm = 0x3FF00000
	movl	$0, 32(%esp)
	jmp	LBB0_3
LBB0_2:                                 # %else
	movsd	24(%esp), %xmm0
	addsd	LCPI0_1, %xmm0
	movsd	%xmm0, (%esp)
	calll	_fib
	movsd	24(%esp), %xmm0
	addsd	LCPI0_2, %xmm0
	movsd	%xmm0, (%esp)
	fstpl	16(%esp)
	calll	_fib
	fstpl	8(%esp)
	movsd	16(%esp), %xmm0
	addsd	8(%esp), %xmm0
	movsd	%xmm0, 32(%esp)
LBB0_3:                                 # %exit
	fldl	32(%esp)
	movl	%ebp, %esp
	popl	%ebp
	ret

	.def	 _main;
	.scl	2;
	.type	32;
	.endef
	.section	.rdata,"r"
	.align	8
LCPI1_0:
	.quad	4621819117588971520     # double 1.000000e+01
                                        #  (0x0)
LCPI1_1:
	.quad	4607182418800017408     # double 1.000000e+00
                                        #  (0x0)
	.text
	.globl	_main
	.align	16, 0x90
_main:                                  # @main
# BB#0:                                 # %func
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-8, %esp
	subl	$32, %esp
	movl	$0, 20(%esp)
	movl	$0, 16(%esp)
	jmp	LBB1_1
	.align	16, 0x90
LBB1_2:                                 # %body
                                        #   in Loop: Header=BB1_1 Depth=1
	movsd	16(%esp), %xmm0
	movsd	%xmm0, (%esp)
	calll	_fib
	fstpl	4(%esp)
	movl	$L___unnamed_1, (%esp)
	calll	_printf
	movsd	16(%esp), %xmm0
	addsd	LCPI1_1, %xmm0
	movsd	%xmm0, 16(%esp)
LBB1_1:                                 # %loop
                                        # =>This Inner Loop Header: Depth=1
	movsd	16(%esp), %xmm0
	ucomisd	LCPI1_0, %xmm0
	jbe	LBB1_2
# BB#3:                                 # %exit
	movb	31(%esp), %al
	movl	%ebp, %esp
	popl	%ebp
	ret

	.data
L___unnamed_1:                          # @0
	.asciz	 "%f\n"


	.globl	__fltused
