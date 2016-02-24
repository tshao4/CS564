select y.name, 100.00 * y.pop / z.pop 
from (select p.name, sum(r.popestimate2011) as pop
		from pop_estimate_nation_state_pr p, 
			(select q.state, q.popestimate2011
				from pop_estimate_state_age_sex_race_origin q
				where q.age >= 21 and q.sex = 0 and q.origin = 0) r
		where p.state = r.state
		group by p.state) y,

	 (select u.name, sum(w.popestimate2011) as pop
		from pop_estimate_nation_state_pr u, 
			(select v.state, v.popestimate2011
				from pop_estimate_state_age_sex_race_origin v
				where v.sex = 0	and v.origin = 0) w
		where u.state = w.state
		group by u.state) z
where y.name = z.name
group by y.name;