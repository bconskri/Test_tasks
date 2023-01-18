with 

clear_contract_conditions as
(select *
from contract_conditions_bd
where marker_delete<>True and conducted <>False),

is_instament as
(select cc.contract_id, cc.condition_id, cc.condition_dt,
	case when cnt>=2 then 1 else 0 end as is_instament
from (select condition_id, count(payment_dt) as cnt
		from contract_conditions_payment_plan_bd
		group by condition_id
		order by condition_id) as ccpp_cnt 
join clear_contract_conditions as cc 
on ccpp_cnt.condition_id=cc.condition_id
order by is_instament),

prolong as
(select customer_id, contract_renewal_serial_number, count(condition_type) as prolong_count
from (select customer_id, nc.contract_id, issue_dt, nc.renewal_contract_id, operation_type, 
	  condition_type, nc.contract_renewal_serial_number
	from tmp_contract as nc left join contract_conditions_bd as cc on nc.contract_id=cc.contract_id
	order by issue_dt) as t
group by customer_id, contract_renewal_serial_number, condition_type
having condition_type = 'Продление'),

first_issue_dt as
(select customer_id,
min(issue_dt) as first_issue_dt
from tmp_contract
group by customer_id),

plan_dt_loan_amount as
(select condition_id, min(payment_dt), max(payment_dt) as plan_dt, sum(loan_amount) as loan_amount
from contract_conditions_payment_plan_bd as ccpp
group by condition_id),

last_plan_dt as
(select tc.contract_id, tc.contract_renewal_serial_number, max(ccpp.payment_dt) as last_plan_dt
from tmp_contract as tc left join clear_contract_conditions as cc on tc.contract_id = cc.contract_id
	left join contract_conditions_payment_plan_bd as ccpp on cc.condition_id = ccpp.condition_id
group by tc.contract_id, tc.contract_renewal_serial_number
order by tc.contract_id),

close_dt as
(select cc.condition_id, max(cc.condition_dt) as close_dt
from clear_contract_conditions as cc 
left join contract_conditions_payment_plan_bd as ccpp
	on cc.condition_id = ccpp.condition_id
 group by  cc.condition_id),

total_loan_amount as
(select 
 	nc.customer_id,
 	sum(pd.loan_amount) as total_loan_amount,
	max(pd.loan_amount) as max_loan_amount, min (loan_amount) as min_loan_amount,
	max(cc.condition_end_dt- cc.condition_start_dt ) as min_loan_term,
	min(cc.condition_end_dt- cc.condition_start_dt ) as max_loan_term
from tmp_contract as nc
join clear_contract_conditions as cc on nc.contract_id = cc.contract_id
left join plan_dt_loan_amount as pd on cc.condition_id = pd.condition_id
group by nc.customer_id
order by nc.customer_id
),

loan_term as
(select 
 cc.contract_id, cc.condition_id, loan_amount, cc.operation_type, cc.condition_start_dt, 
	cc.condition_end_dt, cc.condition_end_dt- cc.condition_start_dt as loan_term
from clear_contract_conditions as cc 
left join plan_dt_loan_amount as pd on cc.condition_id = pd.condition_id
where operation_type = 'ЗаключениеДоговора'
order by contract_id),

-- min_max_loan_term as
-- (select nc.customer_id, min(lt.loan_term) as min_loan_term, max(lt.loan_term) as max_loan_term
-- from tmp_contract nc join loan_term lt on nc.contract_id = lt.contract_id
-- group by nc.customer_id),

is_closed as
(select contract_id, is_closed
from (select contract_id,
case when status_type in ('Закрыт', 'Договор закрыт с переплатой', 'Переоформлен')
	then 'Закрыт'
end as is_closed
from contract_status_bd) as close
where is_closed='Закрыт'),

showcase as
(select tc.contract_id, tc.contract_code, tc.customer_id, cc.condition_id, tc.subdivision_id, 
	tc.contract_serial_number, tc.contract_renewal_serial_number, 
 	CASE WHEN renewal_contract_id is not null then 1 else 0 END  as is_renewal
--  	ii.is_instament,
-- 	p.prolong_count, f.first_issue_dt, tc.issue_dt, pd.plan_dt, lpd.last_plan_dt, date_trunc('day',cd.close_dt) as close_dt,
-- 	pd.loan_amount, tla.total_loan_amount, tla.min_loan_amount, tla.max_loan_amount,
-- 	lt.loan_term, tla.min_loan_term, tla.max_loan_term, ic.is_closed
from tmp_contract as tc  
join clear_contract_conditions as cc on tc.contract_id = cc.contract_id
join is_instament as ii on (tc.contract_id=ii.contract_id and cc.condition_id=ii.condition_id)
left join prolong as p on (tc.customer_id=p.customer_id and tc.contract_renewal_serial_number = p.contract_renewal_serial_number)
join first_issue_dt as f on tc.customer_id=f.customer_id
left join plan_dt_loan_amount as pd on (cc.condition_id = pd.condition_id)
left join last_plan_dt as lpd on (tc.contract_id = lpd.contract_id) 
join close_dt as cd on (cc.condition_id = cd.condition_id)
left join total_loan_amount as tla on (tc.customer_id = tla.customer_id)
left join loan_term as lt on (tc.contract_id = lt.contract_id and cc.condition_id = lt.condition_id)
left join is_closed as ic on (tc.contract_id = ic.contract_id)
order by issue_dt)

-- задание 1
-- cust_19 as
-- (select *
-- from showcase
-- where first_issue_dt between '2019-01-01' and '2019-12-31'),

-- задание 2
-- (select row_number() over (partition by date_trunc('month',first_issue_dt)), count(distinct customer_id)
-- from cust_19
-- group by date_trunc('month',first_issue_dt))



-- t as 
-- (select distinct customer_id, max(contract_serial_number) as max_cont, min(first_issue_dt) as first_issue_dt
-- from cust_19
-- group by customer_id)

-- select customer_id, f.first_issue_dt, f.max_cont
-- from (select date_trunc('month',first_issue_dt) as first_issue_dt, max(max_cont) as max_cont
-- from t
-- group by date_trunc('month',first_issue_dt)) as f join t 
-- on (t.first_issue_dt = f.first_issue_dt and t.max_cont = f.max_cont)

select *
-- contract_id, count (condition_dt)
from 
-- loan_term
-- clear_contract_conditions
-- where operation_type = 'ЗаключениеДоговора' 
-- group by contract_id
-- order by count (condition_dt) desc

-- cc.condition_id = '32d904f7-ae92-11ec-b81f-3cfdfed12dbd'

-- condition_id = '32d904f7-ae92-11ec-b81f-3cfdfed12dbd'
--  contract_id = '32d9039f-ae92-11ec-b81f-3cfdfed12dbd'
--  group by condition_id
--  order by cc.condition_dt
-- contract_conditions_payment_plan_bd
showcase