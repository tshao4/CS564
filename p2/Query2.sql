select p.state, p.births2012
from pop_estimate_nation_state_pr p
where p.births2012 > 80000 and p.state < 57 and p.state > 0