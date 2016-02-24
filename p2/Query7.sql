select d.division_desc, group_concat(p.name)
from division d, pop_estimate_nation_state_pr p
where d.division_cd = p.division and p.state > 0 and p.state < 57
group by p.division;