select p.stname, count(p.county)
from pop_estimate_state_county p
where p.county > 0
group by p.stname;