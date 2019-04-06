using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
public class PlayerMovement : MonoBehaviour {

    public Animator animator;
    public CharacterController2D controller;
    public float runSpeed = 40f;
    public float horizontalMove = 0f;
    bool jump = false;
    
    protected Color staminaRechargingColor = new Color(1f, 1f, 0.41176f, 0.6352f);
    protected Color staminaReadyColor = new Color(1f, 1f, 0.41176f, 1f);
    [SerializeField]protected Image staminaBar;
    [SerializeField]protected float maxSprintTime = 1f;
    protected float staminaTimer = 0f;
    protected bool staminaRecharging = false;
    protected bool sprinting = false;
    [SerializeField]protected float sprintIncrease = 2f;
    
    public void RecoverStamina(float amountBack)
    {
        float recovered = staminaTimer+=amountBack;
        staminaTimer = recovered < maxSprintTime ? recovered : maxSprintTime;
    }
    
    void Awake()
    {
        staminaTimer = maxSprintTime;
        staminaBar.color = staminaReadyColor;
    }
    //Method is called once per frame
    void Update () {
        horizontalMove = Input.GetAxisRaw("Horizontal") * runSpeed;
        animator.SetFloat("Speed", Mathf.Abs(horizontalMove));
        if (Input.GetButtonDown("Jump"))
        {
            jump = true;
        }
        if (Input.GetButtonDown("Fire2"))   //check chrouching
        {
            animator.SetBool("crouching", true);
            horizontalMove/=2f;
        }
        else if(Input.GetButtonUp("Fire2"))
        {
            animator.SetBool("crouching", false);
        }
        else if (Input.GetButton("Fire3") && staminaTimer > 0f && !staminaRecharging)
        {
            sprinting = true;
            staminaTimer-=Time.deltaTime;
            staminaBar.fillAmount = (float) staminaTimer/maxSprintTime;
            horizontalMove*=sprintIncrease;
            if(staminaTimer <= 0f)
            {
                staminaRecharging = true;
                staminaBar.color = staminaRechargingColor;
            }
        }
        
        if(!sprinting)
        {
            staminaTimer += (Time.deltaTime/2.5f);
            staminaTimer = staminaTimer < maxSprintTime ? staminaTimer : maxSprintTime;
            staminaBar.fillAmount = (float) staminaTimer/maxSprintTime;
            if(staminaRecharging && staminaTimer >= maxSprintTime)
            {
                staminaRecharging = false;
                staminaBar.color = staminaReadyColor;
            }
        }
        sprinting = false;
    }

    void FixedUpdate () {
        //Moving the character
        
        controller.Move(horizontalMove * Time.fixedDeltaTime, false, jump);
        jump = false;
    }
}
  
    
