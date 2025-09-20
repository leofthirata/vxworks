///////////////// interview questions answers

1. mutex has a locking mechanism, allowing only one task to access the resource at a time
   semaphore, in the other hand, it can act like mutex (binary semaphore) or allow multiple tasks to access the resource with the counting semaphore

2. piece of code designed to perform specific function, has priority and multiple states
3. ready, running, blocked (waiting for resource to be free), suspended (disabled until resumed), delayed (waiting for time period to elapse), deleted
4. when a low prio task is using a resource a higher prio task is trying to access -> the higher prio task will interrupt the low prio and take control
the solution to this is priority inheritance where the low prio gets its prio higher than the task trying to take control
another solution is the priority ceiling where the task using the resource raises its prio to be the highest of all the tasks
5. tasks are mapped with their priorities according to their function importance

6. You are looking for someone to meet the job requirements and I'm an electrical engineer with over 4 years of experience in embedded systems building products with RTOS and C++, familiarity with VxWorks RTOS, testing (unit, integration), debugging, a bit of device drivers, hardware experience, agile methodologies, proactive and problem-solving mentality

It has been some time since I got interested in Wind River because I know someone who works there so I heard good things about the company. Besides it, I can't ignore the reputation as a leader in software solutions.

I'm confident Wind River will provide me challenging projects and dynamic environments where I will be able to contribute and learn, so I'm pretty sure I'm a good fit for the role

7. when multiple tasks depend on each other and they are all hanging waiting to be able to access their resources, so basically, every task is blocking each other
8. taskspawn creates a task and activates the task right away
can use taskinit and taskectivate instead
9. 