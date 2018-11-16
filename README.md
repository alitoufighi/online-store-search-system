# os-ca2
The second project of our Operating Systems course - A searching system for a store

It first starts as a load balancer. You add your input, and it creates worker processes to looks for data in directory provided by you.
When workers are done, they send their data to a presenter process, created by load balancer, to sort and display the resulting data.

