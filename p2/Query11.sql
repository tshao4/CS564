select t.name, t.ctyname, t.popc
from (select s.name, r.ctyname, r.popc, s.popestimate2012
		from (select d.division_desc, p.state, p.ctyname, p.npopchg_2012 as popc
				from pop_estimate_state_county p, division d
				where p.county > 0 and d.division_cd = p.division 
						and p.npopchg_2012 = (select min(q.npopchg_2012) 
												from pop_estimate_state_county q 
												where q.npopchg_2012 > 0 and p.state = q.state) 
				group by state) r,
			pop_estimate_nation_state_pr s
		where r.state = s.state
		order by r.division_desc asc, s.popestimate2012 desc) t;