#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_send(void)
{
  int receiver_pid;
  uint64 msg_ptr; // Dirección del mensaje en espacio de usuario
  
  argint(0, &receiver_pid);
  argaddr(1, &msg_ptr);
  // Obtener argumentos de la llamada al sistema
  if (receiver_pid < 0 || msg_ptr < 0)
    return -1;

  // Copiar el mensaje desde el espacio de usuario al kernel
  char message[128];
  if (copyin(myproc()->pagetable, message, msg_ptr, sizeof(message)) < 0)
    return -1;

  // Manejar la cola de mensajes
  acquire(&queue_lock);

  if ((queue_end + 1) % MSG_QUEUE_SIZE == queue_start) {
    // La cola está llena
    release(&queue_lock);
    return -1;
  }

  msg_queue[queue_end].sender_pid = myproc()->pid;
  safestrcpy(msg_queue[queue_end].content, message, sizeof(message));
  queue_end = (queue_end + 1) % MSG_QUEUE_SIZE;

  wakeup(&msg_queue); // Despertar a cualquier proceso bloqueado
  release(&queue_lock);

  return 0;
}

uint64
sys_receive(void)
{
  uint64 buffer_ptr; // Dirección del buffer en espacio de usuario
  
  argaddr(0, &buffer_ptr);
  // Obtener el argumento de la llamada al sistema
  if (buffer_ptr < 0)
    return -1;

  // Manejar la cola de mensajes
  acquire(&queue_lock);

  while (queue_start == queue_end) {
    // La cola está vacía, bloquear el proceso
    sleep(&msg_queue, &queue_lock);
  }

  int sender_pid = msg_queue[queue_start].sender_pid;
  char message[128];
  safestrcpy(message, msg_queue[queue_start].content, sizeof(message));
  queue_start = (queue_start + 1) % MSG_QUEUE_SIZE;

  release(&queue_lock);

  // Copiar el mensaje al espacio de usuario
  if (copyout(myproc()->pagetable, buffer_ptr, message, sizeof(message)) < 0)
    return -1;

  return sender_pid;
}
