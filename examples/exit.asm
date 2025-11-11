format PE64 console
entry start

section '.text' code readable executable
    start:
        ;; allocate stack
        sub rsp, 16
        ;; i64
        mov qword [rsp-0], 0
        ;; exit
        mov rcx, [rsp-0]
        sub rsp, 48
        call [KERNEL32.ExitProcess]

section '.idata' import data readable writeable
    dd 0, 0, 0, RVA KERNEL32.NAME, RVA KERNEL32.TABLE
    dd 0, 0, 0, 0, 0

    KERNEL32.TABLE:
        KERNEL32.ExitProcess dq RVA KERNEL32.ExitProcess.NAME, 0

    KERNEL32.NAME: db 'kernel32.dll', 0

    KERNEL32.ExitProcess.NAME:
        dw 0
        db 'ExitProcess', 0
