select q.name, sum(r.popestimate2011)
from pop_estimate_nation_state_pr q,
	 (select p.state, p.popestimate2011
		from pop_estimate_state_age_sex_race_origin p
		where p.age >= 21 and p.age <= 45 and p.sex = 2 and p.origin = 0) r
where q.state = r.state
group by q.state;